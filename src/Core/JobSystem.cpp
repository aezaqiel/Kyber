#include "JobSystem.hpp"

namespace Kyber::Core {

    void JobSystem::Init(u32 workerCount)
    {
        if (m_JobQueue) {
            LOG_WARN("JobSystem::Init() called, but system is already initialized");
            return;
        }

        if (workerCount == 0) {
            m_WorkerCount = std::max(1u, std::thread::hardware_concurrency() - 1);
        } else {
            m_WorkerCount = workerCount;
        }

        LOG_INFO("Initializing job system with {} workers", m_WorkerCount);

        m_JobQueue = std::make_unique<DSA::MPMCQueue<Job>>(2048);
        m_StopSource = std::stop_source();

        m_Workers.reserve(m_WorkerCount);
        for (u32 i = 0; i < m_WorkerCount; ++i) {
            m_Workers.emplace_back(WorkerLoop, m_StopSource.get_token());
        }
    }

    void JobSystem::Shutdown()
    {
        if (!m_JobQueue) {
            return;
        }

        LOG_INFO("Shutting down job system");

        m_StopSource.request_stop();

        for (u32 i = 0; i < m_WorkerCount; ++i) {
            m_JobQueue->Emplace(nullptr);
        }

        m_Workers.clear();

        m_JobQueue.reset();
        LOG_INFO("job system shut down");
    }

    void JobSystem::WorkerLoop(std::stop_token stop)
    {
        Job job;
        while (!stop.stop_requested()) {
            m_JobQueue->Pop(job);
            if (job) {
                job();
            } else {
                if (stop.stop_requested()) {
                    break;
                }
            }
        }
    }

}
