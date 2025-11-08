#pragma once

#include "Timer.hpp"
#include "Events/CoreEvents.hpp"
#include "Window.hpp"

namespace Core {

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
        std::shared_ptr<Window> m_Window;

        inline static std::vector<EventListenerFn> s_EventListeners;
    };

}
