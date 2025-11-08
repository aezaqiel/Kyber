#pragma once

#include "JobCounter.hpp"

namespace Kyber::Core {

    class JobSystem
    {
        friend class Application;

        using Job = std::function<void()>;
    public:
        JobSystem();
        ~JobSystem();

        JobSystem(const JobSystem&) = delete;
        JobSystem& operator=(const JobSystem&) = delete;
        JobSystem(JobSystem&&) = delete;
        JobSystem& operator=(JobSystem&&) = delete;

        void Submit(Job&& job, JobCounter* counter);
        void Wait(JobCounter& counter);

    private:
        void WorkerLoop();

    private:
        std::atomic<bool> m_Running = true;
        std::vector<std::thread> m_Workers;

        std::deque<Job> m_JobQueue;
        std::mutex m_JobQueueMutex;
        std::condition_variable m_JobQueueCV;
    };

}
