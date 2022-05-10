// Folder: Memory

#include "EnginePCH.h"
#include "LeakDetector.h"

LeakDetector* LeakDetector::m_pInstance = nullptr;
thread_local bool LeakDetector::m_NextAllocationAllowLeak = false;
thread_local bool LeakDetector::m_IgnoreRemoveAllocation = false;

void LeakDetector::Initialize()
{
	m_pInstance = new LeakDetector();
}

LeakDetector* LeakDetector::GetInstance()
{
	return m_pInstance;
}

LeakDetector::~LeakDetector()
{
	m_pInstance = nullptr;
}

LeakDetector::AllocationData& LeakDetector::AddAllocation(void* pMemoryAddress, uint allocatedBytes, const std::string& fileName, uint fileLine, bool isLeakAllowed)
{
	if (m_NextAllocationAllowLeak)
	{
		isLeakAllowed = true;
		m_NextAllocationAllowLeak = false;
	}

	AllocationData data;
	data.isLeakAllowed = isLeakAllowed;
	data.allocatedBytes = allocatedBytes;
	data.fileName = fileName;
	data.fileLine = fileLine;
	data.deleteFunc = nullptr;

	std::scoped_lock lock(m_AllocationsMutex);
	RAIIMod tempMod(m_IgnoreRemoveAllocation, true); // Necessary to avoid deadlock in RemoveAllocation because .emplace might call delete operator internally
	return m_Allocations.emplace(pMemoryAddress, data).first->second;
}

void LeakDetector::RemoveAllocation(void* pMemoryAddress, uint freedBytes)
{
	if (m_IgnoreRemoveAllocation)
		return;

	std::scoped_lock lock(m_AllocationsMutex);
	RAIIMod tempMod(m_IgnoreRemoveAllocation, true); // Necessary to avoid deadlock in RemoveAllocation because .emplace and .erase might call delete operator internally

	if (m_Allocations.find(pMemoryAddress) == m_Allocations.end())
		return;

	const AllocationData& data = m_Allocations[pMemoryAddress];
	if (freedBytes == UINT_MAX)
		freedBytes = data.allocatedBytes;

	if (data.allocatedBytes < freedBytes)
	{
		LOG_CRITICAL("Trying to remove allocation in the leak detector of byte_count=%d, while the allocated size is only %d", freedBytes, data.allocatedBytes);
	}

	if (data.allocatedBytes > freedBytes)
	{
		AllocationData newData;
		newData.allocatedBytes = data.allocatedBytes - freedBytes;
		newData.fileName = data.fileName;
		newData.fileLine = data.fileLine;

		m_Allocations.emplace(reinterpret_cast<char*>(pMemoryAddress) + freedBytes, newData);
		LOG_CRITICAL("Removing allocation in the leak detector of byte_count=%d, while the allocated size is %d", freedBytes, data.allocatedBytes);
	}

	m_Allocations.erase(pMemoryAddress);
}

void LeakDetector::DumpLeaks()
{
	LOG_DEBUG("\n--------------------------------------------------------------------");
	LOG_DEBUG(  "------------------------- [ MEMORY LEAKS ] -------------------------");
	LOG_DEBUG(  "--------------------------------------------------------------------\n");

	// Delete the allowed leak allocations
	uint totalBytesLeakedAllowed = 0;
	std::vector<void*> toDeleteAllowedAllocations;
	for (const std::pair<void*, AllocationData>& pair : m_Allocations)
	{
		if (pair.second.isLeakAllowed)
		{
			totalBytesLeakedAllowed += pair.second.allocatedBytes;
			toDeleteAllowedAllocations.push_back(pair.first);
		}
	}
	for (void* pToDeleteAllocation : toDeleteAllowedAllocations)
	{
		if (m_Allocations[pToDeleteAllocation].deleteFunc != nullptr)
			m_Allocations[pToDeleteAllocation].deleteFunc(pToDeleteAllocation); // deleteFunc will cast void* to actual type* and delete it
	}

	// Detect and report leaks
	uint totalBytesLeaked = 0;
	for(const std::pair<void*, AllocationData>& pair : m_Allocations)
	{
		void* pMemoryAddress = pair.first;
		const AllocationData& data = pair.second;

		totalBytesLeaked += data.allocatedBytes;

#pragma warning(push)
#pragma warning(disable: 4311)
#pragma warning(disable: 4302)
		LOG_ERROR("Leak detected: 0x%.8X\t%d bytes\t%s::%d", (uint)(uintptr_t)pMemoryAddress, data.allocatedBytes, data.fileName.c_str(), data.fileLine);
#pragma warning(pop)
	}

	if (totalBytesLeaked == 0)
	{
#ifdef ENABLE_LEAK_DETECTION
		LOG_DEBUG("                           No leaks found                           ");
#else
		LOG_DEBUG("                     Leak detection not enabled                     ");
#endif
	}

	LOG_DEBUG("\n--------------------- [ MEMORY LEAKS SUMMARY ] ---------------------");
	LOG_DEBUG(  "Total leaked bytes: %d", totalBytesLeakedAllowed + totalBytesLeaked);
	LOG_DEBUG(  "--> Allowed: %d bytes", totalBytesLeakedAllowed);
	LOG_DEBUG(  "--> Real leaks: %d bytes", totalBytesLeaked);
	LOG_DEBUG(  "--------------------------------------------------------------------\n");

	// Deleting leak detector instance
	LeakDetector* pTempInstance = m_pInstance;
	m_pInstance = nullptr;
	SAFE_DELETE(pTempInstance);
}