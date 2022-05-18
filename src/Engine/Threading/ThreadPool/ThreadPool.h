// Folder: Threading/ThreadPool

#pragma once

class ThreadPool
{
public:
	typedef std::function<void()> WorkFunc;

	static constexpr const uint INVALID_TASK_IDENTIFIER = 0;

public:
	static ThreadPool* GetInstance();
	~ThreadPool();

	void Flush();
	uint QueueWork(WorkFunc&& work);
	bool CancelTask(uint identifier);

	bool IsTaskQueued(uint identifier) const;
	bool IsTaskExecuting(uint identifier) const;
	bool IsTaskFinished(uint identifier) const;

private:
	ThreadPool();

	static void WorkerRun(ThreadPool* pThreadPool);

private:
	static const uint THREAD_COUNT;

	bool m_IsShuttingDown;
	bool m_IsFlushing;

	std::vector<std::thread> m_Threads;

	mutable std::mutex m_WorkQueueMutex;
	std::queue<std::pair<WorkFunc, uint>> m_WorkQueue;
	std::condition_variable m_ConditionVariable;

	std::atomic_uint8_t m_FlushCounter;
	std::condition_variable m_FlushFinishedConditionVariable;

	uint m_PreviousTaskIdentifier;
	std::unordered_map<uint, std::reference_wrapper<WorkFunc>> m_QueuedTaskIdentifiers;
	std::unordered_set<uint> m_ExecutingTaskIdentifiers;
};