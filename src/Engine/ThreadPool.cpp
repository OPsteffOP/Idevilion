// Folder: Threading/ThreadPool

#include "EnginePCH.h"
#include "ThreadPool.h"

const uint ThreadPool::THREAD_COUNT = std::thread::hardware_concurrency();

ThreadPool* ThreadPool::GetInstance()
{
	static ThreadPool* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInstance = new ThreadPool();
	}

	return pInstance;
}

ThreadPool::ThreadPool()
	: m_IsShuttingDown(false)
	, m_IsFlushing(false)
	, m_PreviousTaskIdentifier(0)
{
	m_Threads.reserve(THREAD_COUNT);
	for (uint i = 0; i < THREAD_COUNT; ++i)
	{
		m_Threads.emplace_back(WorkerRun, this);
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock lock(m_WorkQueueMutex);
		m_IsShuttingDown = true;
	}

	m_ConditionVariable.notify_all();
	for (std::thread& thread : m_Threads)
	{
		thread.join();
	}
}

void ThreadPool::Flush()
{
	{
		std::unique_lock lock(m_WorkQueueMutex);
		m_IsFlushing = true;
		m_WorkQueue = std::queue<std::pair<WorkFunc, uint>>();
	}

	// Wait until all threads finished their work before continuing
	m_ConditionVariable.notify_all();
	while (m_FlushCounter.load() != THREAD_COUNT);

	{
		std::unique_lock lock(m_WorkQueueMutex);
		m_FlushCounter.store(0);
		m_IsFlushing = false;
		m_FlushFinishedConditionVariable.notify_all();
	}
}

uint ThreadPool::QueueWork(WorkFunc&& work)
{
	uint identifier = 0;

	{
		std::unique_lock lock(m_WorkQueueMutex);
		if (m_IsShuttingDown || m_IsFlushing)
			return INVALID_TASK_IDENTIFIER;

		identifier = ++m_PreviousTaskIdentifier;
		m_WorkQueue.emplace(std::move(work), identifier);
	}
	m_ConditionVariable.notify_one();

	return identifier;
}

bool ThreadPool::CancelTask(uint identifier)
{
	if (identifier == INVALID_TASK_IDENTIFIER)
		return false;

	std::unique_lock lock(m_WorkQueueMutex);

	if (m_QueuedTaskIdentifiers.find(identifier) == m_QueuedTaskIdentifiers.end())
		return false;

	WorkFunc& taskRef = m_QueuedTaskIdentifiers.at(identifier).get();
	taskRef = nullptr;

	m_QueuedTaskIdentifiers.erase(identifier);
	return true;
}

bool ThreadPool::IsTaskQueued(uint identifier) const
{
	if (identifier == INVALID_TASK_IDENTIFIER)
		return false;

	std::unique_lock lock(m_WorkQueueMutex);
	return m_QueuedTaskIdentifiers.find(identifier) != m_QueuedTaskIdentifiers.end();
}

bool ThreadPool::IsTaskExecuting(uint identifier) const
{
	if (identifier == INVALID_TASK_IDENTIFIER)
		return false;

	std::unique_lock lock(m_WorkQueueMutex);
	return m_ExecutingTaskIdentifiers.find(identifier) != m_ExecutingTaskIdentifiers.end();
}

bool ThreadPool::IsTaskFinished(uint identifier) const
{
	if (identifier == INVALID_TASK_IDENTIFIER)
		return true;

	std::unique_lock lock(m_WorkQueueMutex);
	return m_QueuedTaskIdentifiers.find(identifier) == m_QueuedTaskIdentifiers.end() &&
		m_ExecutingTaskIdentifiers.find(identifier) == m_ExecutingTaskIdentifiers.end();
}

void ThreadPool::WorkerRun(ThreadPool* pThreadPool)
{
	while (!pThreadPool->m_IsShuttingDown)
	{
		uint taskIdentifier = INVALID_TASK_IDENTIFIER;
		ThreadPool::WorkFunc workFunc;

		{
			std::unique_lock lock(pThreadPool->m_WorkQueueMutex);

			if (pThreadPool->m_IsShuttingDown)
				return;

			if (pThreadPool->m_IsFlushing)
			{
				pThreadPool->m_FlushCounter.fetch_add(1);
				pThreadPool->m_FlushFinishedConditionVariable.wait(lock);
				continue;
			}

			pThreadPool->m_ConditionVariable.wait(lock);

			if (pThreadPool->m_IsShuttingDown || pThreadPool->m_IsFlushing || pThreadPool->m_WorkQueue.empty())
				continue;

			workFunc = std::move(pThreadPool->m_WorkQueue.front().first);

			if (workFunc != nullptr)
			{
				taskIdentifier = pThreadPool->m_WorkQueue.front().second;
				pThreadPool->m_QueuedTaskIdentifiers.erase(taskIdentifier);
				pThreadPool->m_ExecutingTaskIdentifiers.emplace(taskIdentifier);
			}

			pThreadPool->m_WorkQueue.pop();
		}

		if (workFunc != nullptr)
		{
			workFunc();

			std::unique_lock lock(pThreadPool->m_WorkQueueMutex);
			pThreadPool->m_ExecutingTaskIdentifiers.erase(taskIdentifier);
		}
	}
}