#pragma once

namespace Kyber::DSA {

    template <typename T, typename Allocator = std::allocator<T>>
    class MPMCQueue
    {
    public:
        explicit MPMCQueue(usize capacity, const Allocator& alloc = Allocator())
            : m_Capacity(capacity), m_Allocator(alloc)
        {
            m_Buffer = std::allocator_traits<SlotAllocator>::allocate(m_SlotAllocator, m_Capacity);

            for (usize i = 0; i < m_Capacity; ++i) {
                std::construct_at(&m_Buffer[i]);
                m_Buffer[i].sequence.store(i, std::memory_order_relaxed);
            }
        }

        ~MPMCQueue()
        {
            T out;
            while (TryPop()) {
            }

            for (usize i = 0; i < m_Capacity; ++i) {
                std::destroy_at(&m_Buffer[i]);
            }

            std::allocator_traits<SlotAllocator>::deallocate(m_SlotAllocator, m_Buffer, m_Capacity);
        }

        MPMCQueue(const MPMCQueue&) = delete;
        MPMCQueue& operator=(const MPMCQueue&) = delete;
        MPMCQueue(MPMCQueue&&) = delete;
        MPMCQueue& operator=(MPMCQueue&&) = delete;

        template <typename... Args>
        requires (std::is_constructible_v<T, Args&&...>)
        void Emplace(Args&&... args) noexcept
        {
            const usize pos = m_Head.fetch_add(1, std::memory_order_relaxed);

            Slot& slot = m_Buffer[pos % m_Capacity];

            usize expectedSeq = pos;
            while (true) {
                usize currentSeq = slot.sequence.load(std::memory_order_acquire);
                if (currentSeq == expectedSeq) {
                    break;
                }
                slot.sequence.wait(currentSeq, std::memory_order_relaxed);
            }

            std::construct_at(slot.GetData(), std::forward<Args>(args)...);

            slot.sequence.store(pos + 1, std::memory_order_release);
            slot.sequence.notify_one();
        }

        void Push(const T& value) requires (std::is_copy_constructible_v<T>)
        {
            Emplace(value);
        }

        void Push(T&& value) requires (std::is_move_constructible_v<T>)
        {
            Emplace(value);
        }

        T Pop() noexcept
        {
            const usize pos = m_Tail.fetch_add(1, std::memory_order_relaxed);

            Slot& slot = m_Buffer[pos % m_Capacity];

            usize expectedSeq = pos + 1;
            while (true) {
                usize currentSeq = slot.sequence.load(std::memory_order_acquire);
                if (currentSeq == expectedSeq) {
                    break;
                }
                slot.sequence.wait(currentSeq, std::memory_order_relaxed);
            }

            T out = std::move(*slot.GetData());

            std::destroy_at(slot.GetData());

            slot.sequence.store(pos + m_Capacity, std::memory_order_release);
            slot.sequence.notify_one();

            return out;
        }

        template <typename... Args>
        requires (std::is_constructible_v<T, Args&&...>)
        bool TryEmplace(Args&&... args) noexcept
        {
            usize pos = m_Head.load(std::memory_order_relaxed);

            Slot& slot = m_Buffer[pos % m_Capacity];

            if (slot.sequence.load(std::memory_order_acquire) != pos) {
                return false;
            }

            if (!m_Head.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                return false;
            }

            std::construct_at(slot.GetData(), std::forward<Args>(args)...);

            slot.sequence.store(pos + 1, std::memory_order_release);
            slot.sequence.notify_one();

            return true;
        }

        std::optional<T> TryPop() noexcept
        {
            usize pos = m_Tail.load(std::memory_order_relaxed);

            Slot& slot = m_Buffer[pos % m_Capacity];

            if (slot.sequence.load(std::memory_order_acquire) != pos + 1) {
                return std::nullopt;
            }

            if (!m_Tail.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                return std::nullopt;
            }

            std::optional<T> result = std::move(*slot.GetData());
            std::destroy_at(slot.GetData());

            slot.sequence.store(pos + m_Capacity, std::memory_order_release);
            slot.sequence.notify_one();

            return result;
        }

        usize SizeApprox() const
        {
            return m_Head.load(std::memory_order_relaxed) - m_Tail.load(std::memory_order_relaxed);
        }

    private:
        inline static constexpr usize CACHE_LINE_SIZE = std::hardware_destructive_interference_size;

        struct alignas(CACHE_LINE_SIZE) Slot
        {
            std::atomic<usize> sequence = 0;
            alignas(alignof(T)) std::byte data[sizeof(T)];

            T* GetData() { return reinterpret_cast<T*>(data); }
            const T* GetData() const { return reinterpret_cast<const T*>(data); }
        };

        using SlotAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Slot>;

        const usize m_Capacity;
        Slot* m_Buffer;

        Allocator m_Allocator;
        SlotAllocator m_SlotAllocator = m_Allocator;

        alignas(CACHE_LINE_SIZE) std::atomic<usize> m_Head = 0;
        alignas(CACHE_LINE_SIZE) std::atomic<usize> m_Tail = 0;
    };

}
