#include "JobSystem.hpp"

namespace Kyber::Core {

    JobSystem::JobSystem()
    {
        const u32 numWorker = std::max(1u, std::thread::hardware_concurrency() - 2);
        LOG_INFO("Initializing job system with {} workers", numWorker);

        for (u32 i = 0; i < numWorker; ++i) {
            m_Workers.emplace_back([this] { WorkerLoop(); });
        }
    }

    JobSystem::~JobSystem()
    {
        m_Running = false;
        m_JobQueueCV.notify_all();
        for (auto& thread : m_Workers) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void JobSystem::Submit(Job&& job, JobCounter* counter)
    {
        if (counter) {
            counter->Increment();
        }

        Job jobWrapper = [job = std::move(job), counter]() {
            job();
            if (counter) {
                counter->Decrement();
            }
        };

        {
            std::lock_guard lock(m_JobQueueMutex);
            m_JobQueue.push_back(std::move(jobWrapper));
        }

        m_JobQueueCV.notify_one();
    }

    void JobSystem::Wait(JobCounter& counter)
    {
        counter.Wait();
    }

    void JobSystem::WorkerLoop()
    {
        while (m_Running) {
            Job job;
            {
                std::unique_lock lock(m_JobQueueMutex);
                m_JobQueueCV.wait(lock, [&] {
                    return !m_JobQueue.empty() || !m_Running;
                });

                if (!m_Running && m_JobQueue.empty()) {
                    return;
                }

                job = std::move(m_JobQueue.front());
                m_JobQueue.pop_front();
            }

            job();
        }
    }

}

