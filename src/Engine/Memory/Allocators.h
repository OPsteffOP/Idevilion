// Folder: Memory

#pragma once
#include "LeakDetector.h"

#ifdef ENABLE_LEAK_DETECTION
#pragma warning(push)
#pragma warning(disable: 4595)
inline void operator delete(void* ptr)
{
	if (LeakDetector::IsInitialized())
		LeakDetector::GetInstance()->RemoveAllocation(ptr);
	std::free(ptr);
};

inline void operator delete[](void* ptr)
{
	if (LeakDetector::IsInitialized())
		LeakDetector::GetInstance()->RemoveAllocation(ptr);
	std::free(ptr);
};
#pragma warning(pop)

#define ALLOW_NEXT_ALLOCATION_LEAK() LeakDetector::GetInstance()->AllowNextAllocationLeak()
#define new LeakDetectorAllocator(__FILE__, __LINE__) << new
#define delete delete
#else
#define ALLOW_NEXT_ALLOCATION_LEAK()
#endif