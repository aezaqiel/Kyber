#pragma once

#include "Timer.hpp"
#include "Window.hpp"
#include "Events.hpp"

#include "Scene/Camera.hpp"

#include "Renderer/Renderer.hpp"

namespace Kyber::Core {

    class Application
    {
    public:
        Application();
        ~Application() = default;

        void Run();

    private:
        void ProcessEvents();
    
    private:
        bool m_Running { true };
        bool m_Minimized { false };

        std::unique_ptr<Timer> m_Timer;
        std::unique_ptr<EventQueue> m_EventQueue;
        std::shared_ptr<Window> m_Window;

        std::unique_ptr<Scene::Camera> m_Camera;

        std::unique_ptr<Renderer::Renderer> m_Renderer;
    };

}
