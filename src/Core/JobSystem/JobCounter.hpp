#pragma once

namespace Kyber::Core {

    class JobSystem;

    class JobCounter
    {
        friend class JobSystem;
    public:
        constexpr JobCounter()
            : m_Counter(0)
        {
        }

        inline void Wait()
        {
            std::unique_lock lock(m_Mutex);
            m_Condition.wait(lock, [&] { return m_Counter.load() == 0; });
        }

    private:
        inline void Increment()
        {
            m_Counter.fetch_add(1);
        }

        inline void Decrement()
        {
            if (m_Counter.fetch_sub(1) == 1) {
                std::lock_guard lock(m_Mutex);
                m_Condition.notify_all();
            }
        }

    private:
        std::atomic<u32> m_Counter;
        std::mutex m_Mutex;
        std::condition_variable m_Condition;
    };

}
