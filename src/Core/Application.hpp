#pragma once

#include "Timer.hpp"
#include "Window.hpp"
#include "Events/CoreEvents.hpp"
#include "JobSystem/JobSystem.hpp"

namespace Kyber::Core {

    class Application
    {
    public:
        using EventListenerFn = std::function<void(EventDispatcher<CoreEvents>&)>;

    public:
        Application();
        ~Application() = default;

        void Run();

        inline static void RegisterOnEvent(const EventListenerFn& fn)
        {
            s_EventListeners.push_back(fn);
        }

    private:
        void ProcessEvents();
    
    private:
        bool m_Running { true };
        bool m_Minimized { false };

        std::unique_ptr<Timer> m_Timer;

        std::unique_ptr<EventQueue<CoreEvents>> m_EventQueue;
        inline static std::vector<EventListenerFn> s_EventListeners;

        std::shared_ptr<Window> m_Window;
        std::shared_ptr<JobSystem> m_JobSystem;
    };

}
