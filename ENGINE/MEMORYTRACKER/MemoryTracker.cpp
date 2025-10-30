#include "MemoryTracker.h"

MemoryTracker::MemoryTracker()
    : memoryTrackerContainer{}
{
}

MemoryTracker::~MemoryTracker()
{
}

bool MemoryTracker::AddMemoryTrackerEntry(const std::string& processName)
{
    if (memoryTrackerContainer.find(processName) != memoryTrackerContainer.end())
        return false; // Already exists

    memoryTrackerContainer.emplace(processName, TrackerInfo(processName));
    return true;
}

bool MemoryTracker::AddEndForMemoryTrackerEntry(const std::string& processName)
{
    auto it = memoryTrackerContainer.find(processName);
    if (it == memoryTrackerContainer.end())
        return false; // Tracker not found

    it->second.SetEndPointForTracker();
    return true;
}

void MemoryTracker::PrettyPrintMemoryAllocationForTrackers()
{
#ifdef ENABLE_MEMORY_TRACKER_INFO
    std::cout << "### Summary Of Memory Allocation Of All The Trackers ###\n";
    for (const auto& it : memoryTrackerContainer)
    {
        std::cout << "Tracker Name : " << it.first
            << " | Memory Consumed : " << it.second.GetMemoryConsumed()
            << " MB\n";
    }
#endif
}

void MemoryTracker::PrintCurrentMemoryUsage()
{
#ifdef ENABLE_MEMORY_TRACKER_INFO
    std::cout << "The Current Memory Usage is : "
        << MemoryHelper::GetCurrentMemoryUsageMB()
        << " MB" << std::endl;
#endif
}