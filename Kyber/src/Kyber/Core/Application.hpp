#pragma once

#include "Window.hpp"

namespace Kyber {

    class Application
    {
    public:
        Application();
        virtual ~Application();

        auto Run() -> void;

    private:
        auto DispatchEvents(const Event& event) -> void;

    private:
        bool m_Running { true };
        bool m_Minimized { false };

        std::unique_ptr<Window> m_Window;
    };

    Application* CreateApplication();

}
