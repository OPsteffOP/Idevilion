// Folder: Memory

#pragma once

class LeakDetector
{
	friend class LeakDetectorAllocator;

public:
	struct AllocationData
	{
		uint allocatedBytes;
		bool isLeakAllowed;

		std::string fileName;
		uint fileLine;

		void(*deleteFunc)(void*);
	};

public:
	static bool IsInitialized() { return m_pInstance != nullptr; }
	static void Initialize();

	static LeakDetector* GetInstance();

	void AllowNextAllocationLeak() { m_NextAllocationAllowLeak = true; }

	AllocationData& AddAllocation(void* pMemoryAddress, uint allocatedBytes, const std::string& fileName, uint fileLine, bool isLeakAllowed = false);
	void RemoveAllocation(void* pMemoryAddress, uint freedBytes = UINT_MAX);

	void DumpLeaks();

private:
	LeakDetector() = default;
	~LeakDetector();

private:
	static LeakDetector* m_pInstance;

	// These need to be thread_local because the value set here will only be valid for the current thread it set the value
	thread_local static bool m_NextAllocationAllowLeak;
	thread_local static bool m_IgnoreRemoveAllocation;

	std::mutex m_AllocationsMutex;
	std::unordered_map<void*, AllocationData> m_Allocations;
};

class LeakDetectorAllocator
{
public:
	LeakDetectorAllocator(const std::string& fileName, uint fileLine)
		: m_FileName(fileName)
		, m_FileLine(fileLine)
		, m_IsLeakAllowed(false)
	{
		if (LeakDetector::GetInstance()->IsInitialized())
		{
			m_IsLeakAllowed = LeakDetector::GetInstance()->m_NextAllocationAllowLeak;
			LeakDetector::GetInstance()->m_NextAllocationAllowLeak = false;
		}
	}

	template<typename T>
	T* operator<<(T* pPtr) const
	{
		if (LeakDetector::IsInitialized())
		{
			LeakDetector::AllocationData& data = LeakDetector::GetInstance()->AddAllocation(pPtr, (uint)sizeof(T), m_FileName, m_FileLine, m_IsLeakAllowed);
			data.deleteFunc = [](void* pPtr) { delete static_cast<T*>(pPtr); }; // Inspired by https://herbsutter.com/2016/09/25/to-store-a-destructor/
		}

		return pPtr;
	}

private:
	const std::string m_FileName;
	const uint m_FileLine;
	bool m_IsLeakAllowed;
};