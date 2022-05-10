// Folder: Threading/ThreadPool

#pragma once

class ThreadPool
{
public:
	typedef std::function<void()> WorkFunc;

public:
	static ThreadPool* GetInstance();
	~ThreadPool();

	void Flush();
	void QueueWork(WorkFunc&& work);

private:
	ThreadPool();

	static void WorkerRun(ThreadPool* pThreadPool);

private:
	static const uint THREAD_COUNT;

	bool m_IsShuttingDown;
	bool m_IsFlushing;

	std::vector<std::thread> m_Threads;

	std::mutex m_WorkQueueMutex;
	std::queue<WorkFunc> m_WorkQueue;
	std::condition_variable m_ConditionVariable;

	std::atomic_uint8_t m_FlushCounter;
	std::condition_variable m_FlushFinishedConditionVariable;
};