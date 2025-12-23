#pragma once

#include "Events.hpp"
#include "Window.hpp"

namespace Kyber {

    class Application
    {
    public:
        struct Config
        {
            Window::Config window;
        };

    public:
        Application(const Config& config);
        ~Application();

        void Run();

    private:
        void DispatchEvents(const Event& event);

    private:
        bool m_Running { true };
        bool m_Minimized { false };

        std::unique_ptr<Window> m_Window;
    };

}
