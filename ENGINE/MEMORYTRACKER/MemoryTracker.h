#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#include <Windows.h>
#include <psapi.h>
#elif defined(__APPLE__) || defined(__MACH__)
#define PLATFORM_MACOS
#include <mach/mach.h>
#elif defined(__linux__)
#define PLATFORM_LINUX
#include <unistd.h>
#include <fstream>
#endif

class MemoryHelper
{
public:
    // Returns current memory usage in megabytes
    static double GetCurrentMemoryUsageMB()
    {
        const double MEGABYTE = 1024.0 * 1024.0;

#ifdef PLATFORM_WINDOWS
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        {
            return static_cast<double>(pmc.WorkingSetSize) / MEGABYTE;
        }
        return 0.0;

#elif defined(PLATFORM_MACOS)
        struct mach_task_basic_info info;
        mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

        if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
            (task_info_t)&info, &infoCount) == KERN_SUCCESS)
        {
            return static_cast<double>(info.resident_size) / MEGABYTE;
        }
        return 0.0;

#elif defined(PLATFORM_LINUX)
        long rss = 0L;
        std::ifstream statm("/proc/self/statm");
        if (statm.is_open())
        {
            std::string dummy;
            statm >> dummy >> rss; // Skip first value, read RSS (in pages)
            statm.close();

            long page_size = sysconf(_SC_PAGESIZE);
            return (rss * page_size) / MEGABYTE;
        }
        return 0.0;

#else
#error "Unsupported platform"
        return 0.0;
#endif
    }
};

struct TrackerInfo
{
    TrackerInfo(const std::string& name_)
        : name(name_)
        , startMemorySize(MemoryHelper::GetCurrentMemoryUsageMB())
        , endMemorySize(0.0)
    {
    }

    void SetEndPointForTracker()
    {
        endMemorySize = MemoryHelper::GetCurrentMemoryUsageMB();
    }

    void UpdateStartMemory(double startMemorySize_)
    {
        startMemorySize = startMemorySize_;
    }

    void UpdateEndMemorySize(double endMemorySize_)
    {
        endMemorySize = endMemorySize_;
    }

    double GetMemoryConsumed() const
    {
        return endMemorySize - startMemorySize;
    }

    const std::string& GetName() const
    {
        return name;
    }

private:
    std::string name;
    double startMemorySize;
    double endMemorySize;
};

class MemoryTracker
{
public:
    MemoryTracker();
    ~MemoryTracker();

    bool AddMemoryTrackerEntry(const std::string& processName);
    bool AddEndForMemoryTrackerEntry(const std::string& processName);
    void PrintCurrentMemoryUsage();
    void PrettyPrintMemoryAllocationForTrackers();

private:
    std::unordered_map<std::string, TrackerInfo> memoryTrackerContainer;
};