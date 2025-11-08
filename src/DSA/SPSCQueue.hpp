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
            m_Head.store(0, std::memory_order_relaxed);
            m_Tail.store(0, std::memory_order_relaxed);
        }

        ~SPSCQueue()
        {
            while (!IsEmpty()) {
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
        bool Emplace(Args&&... args)
        {
            const usize currentTail = m_Tail.load(std::memory_order_relaxed);
            const usize nextTail = (currentTail + 1) % m_Capacity;

            if (nextTail == m_Head.load(std::memory_order_acquire)) {
                return false;
            }

            std::construct_at(GetSlot(currentTail), std::forward<Args>(args)...);

            m_Tail.store(nextTail, std::memory_order_release);

            return true;
        }

        bool Push(const T& value) requires (std::is_copy_constructible_v<T>)
        {
            return Emplace(value);
        }

        bool Push(T&& value) requires (std::is_move_constructible_v<T>)
        {
            return Emplace(std::move(value));
        }

        bool Pop(T& out)
        {
            const usize currentHead = m_Head.load(std::memory_order_relaxed);

            if (currentHead == m_Tail.load(std::memory_order_acquire)) {
                return false;
            }

            T* slot = GetSlot(currentHead);
            out = std::move(*slot);

            std::destroy_at(slot);

            m_Head.store((currentHead + 1) % m_Capacity, std::memory_order_release);

            return true;
        }

        bool IsEmpty() const
        {
            return m_Tail.load(std::memory_order_relaxed) == m_Head.load(std::memory_order_relaxed);
        }

        usize Size() const
        {
            const usize currentTail = m_Tail.load(std::memory_order_relaxed);
            const usize currentHead = m_Head.load(std::memory_order_relaxed);

            if (currentTail >= currentHead) {
                return currentTail - currentHead;
            }

            return m_Capacity - (currentHead - currentTail);
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

        alignas(CACHE_LINE_SIZE) std::atomic<usize> m_Head;
        char pad0_[CACHE_LINE_SIZE - sizeof(std::atomic<usize>)];

        alignas(CACHE_LINE_SIZE) std::atomic<usize> m_Tail;
        char pad1_[CACHE_LINE_SIZE - sizeof(std::atomic<usize>)];
    };

}
