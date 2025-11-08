#include "Application.hpp"

#include "Input.hpp"

namespace Kyber::Core {

    Application::Application()
    {
        m_Timer = std::make_unique<Timer>();

        m_EventQueue = std::make_unique<EventQueue>(1024);

        m_Window = std::make_shared<Window>(Window::Config(1280, 720, "Kyber"));
        m_Window->BindEventQueue(m_EventQueue.get());

        m_Renderer = std::make_unique<Renderer::Renderer>(m_Window);
    }

    void Application::Run()
    {
        while (m_Running) {
            m_Timer->Tick();

            Window::PollEvents();
            Input::Update();

            ProcessEvents();

            // NOTE: Maybe we dont want this?
            if (Input::IsKeyDown(KeyCode::Escape)) {
                m_Running = false;
            }

            if (!m_Minimized) {
                Renderer::RenderPacket packet;

                m_Renderer->SubmitFrame(std::move(packet));
            }
        }
    }

    void Application::ProcessEvents()
    {
        Event event;
        while (m_EventQueue->Pop(event)) {
            EventDispatcher dispatcher(event);

            dispatcher.Dispatch<WindowClosedEvent>([&](const WindowClosedEvent&) {
                m_Running = false;
                return true;
            });

            dispatcher.Dispatch<WindowMinimizeEvent>([&](const WindowMinimizeEvent& e) {
                m_Minimized = e.minimized;
                return false;
            });

            Input::OnEvent(dispatcher);
        }
    }

}
