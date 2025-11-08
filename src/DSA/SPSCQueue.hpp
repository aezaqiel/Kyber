#pragma once

namespace Kyber::DSA {

    template <typename T, typename Allocator = std::allocator<T>>
    class SPSCQueue
    {
    public:
        explicit SPSCQueue(usize capacity, const Allocator& alloc = Allocator())
            : m_Capacity(capacity + 1), m_Allocator(alloc)
        {
            m_Buffer = m_Allocator.allocate(m_Capacity);
        }

        ~SPSCQueue()
        {
            while (!IsEmptyApprox()) {
                std::destroy_at(GetSlot(m_Head.load(std::memory_order_relaxed)));
                m_Head.store(
                    (m_Head.load(std::memory_order_relaxed) + 1) % m_Capacity,
                    std::memory_order_relaxed
                );
            }

            m_Allocator.deallocate(m_Buffer, m_Capacity);
        }

        SPSCQueue(const SPSCQueue&) = delete;
        SPSCQueue& operator=(const SPSCQueue&) = delete;
        SPSCQueue(SPSCQueue&&) = delete;
        SPSCQueue& operator=(SPSCQueue&&) = delete;

        template <typename... Args>
            requires (std::is_constructible_v<T, Args...>)
        bool TryEmplace(Args&&... args)
        {
            const usize currentTail = m_Tail.load(std::memory_order_relaxed);
            const usize nextTail = (currentTail + 1) % m_Capacity;

            if (nextTail == m_Head.load(std::memory_order_acquire)) {
                return false;
            }

            std::construct_at(GetSlot(currentTail), std::forward<Args>(args)...);

            m_Tail.store(nextTail, std::memory_order_release);
            m_Tail.notify_one();

            return true;
        }

        bool TryPush(const T& value) requires (std::is_copy_constructible_v<T>)
        {
            return TryEmplace(value);
        }

        bool TryPush(T&& value) requires (std::is_move_constructible_v<T>)
        {
            return TryEmplace(std::move(value));
        }

        std::optional<T> TryPop()
        {
            const usize currentHead = m_Head.load(std::memory_order_relaxed);

            if (currentHead == m_Tail.load(std::memory_order_acquire)) {
                return std::nullopt;
            }

            T* slot = GetSlot(currentHead);
            T out = std::move(*slot);

            std::destroy_at(slot);

            m_Head.store((currentHead + 1) % m_Capacity, std::memory_order_release);
            m_Head.notify_one();

            return out;
        }

        bool IsEmptyApprox() const
        {
            return m_Tail.load(std::memory_order_relaxed) == m_Head.load(std::memory_order_relaxed);
        }

        usize SizeApprox() const
        {
            const usize currentTail = m_Tail.load(std::memory_order_relaxed);
            const usize currentHead = m_Head.load(std::memory_order_relaxed);

            if (currentTail >= currentHead) {
                return currentTail - currentHead;
            }

            return m_Capacity - (currentHead - currentTail);
        }

        template <typename... Args>
            requires (std::is_constructible_v<T, Args...>)
        void Emplace(Args&&... args)
        {
            while (!TryEmplace(std::forward<Args>(args)...)) {
                std::this_thread::yield();
            }
        }

        void Push(const T& value) requires (std::is_copy_constructible_v<T>)
        {
            Emplace(value);
        }

        void Push(T&& value) requires (std::is_move_constructible_v<T>)
        {
            Emplace(std::move(value));
        }

        T Pop()
        {
            usize currentHead = m_Head.load(std::memory_order_relaxed);
            while (currentHead == m_Tail.load(std::memory_order_acquire)) {
                m_Tail.wait(currentHead, std::memory_order_relaxed);
            }

            T* slot = GetSlot(currentHead);
            T out = std::move(*slot);

            std::destroy_at(slot);

            m_Head.store((currentHead + 1) % m_Capacity, std::memory_order_release);

            return out;
        }

    private:
        T* GetSlot(usize index)
        {
            return m_Buffer + index;
        }

    private:
        inline static constexpr usize CACHE_LINE_SIZE = std::hardware_destructive_interference_size;

        const usize m_Capacity;
        T* m_Buffer;
        Allocator m_Allocator;

        alignas(CACHE_LINE_SIZE) std::atomic<usize> m_Head = 0;
        char pad0_[CACHE_LINE_SIZE - sizeof(std::atomic<usize>)];

        alignas(CACHE_LINE_SIZE) std::atomic<usize> m_Tail = 0;
        char pad1_[CACHE_LINE_SIZE - sizeof(std::atomic<usize>)];
    };

}
