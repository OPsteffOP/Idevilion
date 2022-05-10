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
		m_WorkQueue = std::queue<WorkFunc>();
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

void ThreadPool::QueueWork(WorkFunc&& work)
{
	{
		std::unique_lock lock(m_WorkQueueMutex);
		if (m_IsShuttingDown || m_IsFlushing)
			return;

		m_WorkQueue.push(std::move(work));
	}

	m_ConditionVariable.notify_one();
}

void ThreadPool::WorkerRun(ThreadPool* pThreadPool)
{
	while (!pThreadPool->m_IsShuttingDown)
	{
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

			workFunc = std::move(pThreadPool->m_WorkQueue.front());
			pThreadPool->m_WorkQueue.pop();
		}

		workFunc();
	}
}