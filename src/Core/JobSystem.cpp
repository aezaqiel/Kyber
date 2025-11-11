#include "JobSystem.hpp"

namespace Kyber::Core {

    void JobSystem::Init(u32 workerCount)
    {
        if (s_JobQueue) {
            LOG_WARN("JobSystem::Init() called, but system is already initialized");
            return;
        }

        if (workerCount == 0) {
            s_WorkerCount = std::max(1u, std::thread::hardware_concurrency() - 2);
        } else {
            s_WorkerCount = std::min(workerCount, std::thread::hardware_concurrency() - 2);
        }

        LOG_INFO("Initializing job system with {} workers", s_WorkerCount);

        s_JobQueue = std::make_unique<DSA::MPMCQueue<Job>>(2048);
        s_StopSource = std::stop_source();

        s_Workers.reserve(s_WorkerCount);
        for (u32 i = 0; i < s_WorkerCount; ++i) {
            s_Workers.emplace_back(WorkerLoop, s_StopSource.get_token(), i+1);
        }
    }

    void JobSystem::Shutdown()
    {
        if (!s_JobQueue) {
            return;
        }

        LOG_INFO("Shutting down job system");

        s_StopSource.request_stop();

        for (u32 i = 0; i < s_WorkerCount; ++i) {
            s_JobQueue->Emplace(nullptr);
        }

        s_Workers.clear();

        s_JobQueue.reset();
        LOG_INFO("job system shutdown");
    }

    void JobSystem::WorkerLoop(std::stop_token stop, u32 index)
    {
        s_WorkerIndex = index;
        while (!stop.stop_requested()) {
            if (const auto& job = s_JobQueue->Pop()) {
                job();
            }
        }
    }

}
