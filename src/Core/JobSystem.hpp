#pragma once

#include "DSA/MPMCQueue.hpp"

namespace Kyber::Core {

    class JobSystem
    {
    public:
        static void Init(u32 workerCount = 0);
        static void Shutdown();

        template <typename F>
            requires (std::is_invocable_v<F>)
        inline static void Submit(F&& func)
        {
            if (m_JobQueue) {
                m_JobQueue->Emplace(std::forward<F>(func));
            }
        }

        template <typename F>
            requires (std::is_invocable_v<F, usize>)
        inline static void Dispatch(usize jobCount, F&& func)
        {
            if (jobCount == 0 || !m_JobQueue) {
                return;
            }

            std::latch latch(jobCount);

            for (usize i = 0; i < jobCount; ++i) {
                m_JobQueue->Emplace([i, &latch, jobFunc = std::forward<F>(func)]() {
                    try {
                        jobFunc(i);
                    } catch (const std::exception& e) {
                        LOG_ERROR("JobSystem: Exception in job {}: {}", i, e.what());
                    }

                    latch.count_down();
                });
            }

            latch.wait();
        }

    private:
        static void WorkerLoop(std::stop_token stop);

    private:
        using Job = std::function<void()>;

        inline static std::unique_ptr<DSA::MPMCQueue<Job>> m_JobQueue;
        inline static std::vector<std::jthread> m_Workers;
        inline static std::stop_source m_StopSource;
        inline static u32 m_WorkerCount = 0;
    };

}
