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
            if (s_JobQueue) {
                s_JobQueue->Emplace(std::forward<F>(func));
            }
        }

        template <typename F>
            requires (std::is_invocable_v<F, usize>)
        inline static void Dispatch(usize jobCount, F&& func)
        {
            if (jobCount == 0 || !s_JobQueue) {
                return;
            }

            std::latch latch(jobCount);

            for (usize i = 0; i < jobCount; ++i) {
                s_JobQueue->Emplace([i, &latch, jobFunc = std::forward<F>(func)]() {
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

        inline static u32 GetWorkerCount() { return s_WorkerCount; }
        inline static u32 GetWorkerIndex() { return s_WorkerIndex; }

    private:
        static void WorkerLoop(std::stop_token stop, u32 index);

    private:
        using Job = std::function<void()>;

        inline static std::unique_ptr<DSA::MPMCQueue<Job>> s_JobQueue;
        inline static std::vector<std::jthread> s_Workers;
        inline static std::stop_source s_StopSource;
        inline static u32 s_WorkerCount = 0;

        inline static thread_local u32 s_WorkerIndex = 0;
    };

}
