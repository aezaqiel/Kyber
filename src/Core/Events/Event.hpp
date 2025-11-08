#pragma once

namespace Core {

    struct BaseEvent { bool handled { false }; };

    template <typename T>
    concept IsEvent = requires {
        { std::is_base_of_v<BaseEvent, std::remove_cvref_t<T>> };
        { std::is_copy_constructible_v<std::remove_cvref_t<T>> };
    };

    template <IsEvent... TEvent>
    using EventVariant = std::variant<TEvent...>;

    template <typename TEvent>
    class EventDispatcher
    {
    public:
        constexpr EventDispatcher(TEvent& event) noexcept
            : m_Event(event) {}

        template <IsEvent T, typename Func>
            requires std::is_invocable_r_v<bool, Func, const T&>
        inline void Dispatch(Func&& func) noexcept
        {
            if (T* event = std::get_if<T>(&m_Event)) {
                if (!event->handled) {
                    event->handled = std::invoke(std::forward<Func>(func), *event);
                }
            }
        }

    private:
        TEvent& m_Event;
    };

    template <typename TEvent>
    class EventQueue
    {
    public:
        EventQueue(usize queueSize = 1024) noexcept
            : m_QueueSize(queueSize)
        {
            m_Buffer.resize(m_QueueSize);
        }

        ~EventQueue() = default;

        EventQueue(const EventQueue&) = delete;
        EventQueue& operator=(const EventQueue&) = delete;
        EventQueue(EventQueue&&) = delete;
        EventQueue& operator=(EventQueue&&) = delete;

        template <IsEvent T>
            requires std::is_constructible_v<TEvent, T&&>
        inline void Push(T&& event)
        {
            auto tail = m_Tail.load(std::memory_order_relaxed);
            auto nextTail = (tail + 1) & (m_QueueSize - 1);

            if (nextTail == m_Head.load(std::memory_order_acquire)) {
                LOG_ERROR("Event queue full");
                return;
            }

            m_Buffer[tail] = std::forward<T>(event);
            m_Tail.store(nextTail, std::memory_order_release);
        }

        inline std::vector<TEvent> Poll()
        {
            std::vector<TEvent> polled;
            polled.reserve(m_QueueSize);

            auto head = m_Head.load(std::memory_order_relaxed);
            auto tail = m_Tail.load(std::memory_order_acquire);

            while (head != tail) {
                polled.push_back(std::move(m_Buffer[head]));
                head = (head + 1) & (m_QueueSize - 1);
            }

            m_Head.store(head, std::memory_order_release);

            return polled;
        }

    private:
        alignas(64) std::atomic<usize> m_Head = 0;
        alignas(64) std::atomic<usize> m_Tail = 0;

        usize m_QueueSize;
        std::vector<TEvent> m_Buffer;
    };

}
