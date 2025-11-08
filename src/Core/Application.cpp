#include "Application.hpp"

#include "Input.hpp"

namespace Core {

    Application::Application()
    {
        m_Timer = std::make_unique<Timer>();

        m_EventQueue = std::make_unique<EventQueue<CoreEvents>>();

        m_Window = std::make_shared<Window>(Window::Config(1280, 720, "Renderer"));
        m_Window->BindEventQueue(m_EventQueue.get());

        Input::Init();
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
            }
        }
    }

    void Application::ProcessEvents()
    {
        for (auto& event : m_EventQueue->Poll()) {
            EventDispatcher<CoreEvents> dispatcher(event);

            dispatcher.Dispatch<WindowClosedEvent>([&](const WindowClosedEvent&) {
                m_Running = false;
                return true;
            });

            dispatcher.Dispatch<WindowMinimizeEvent>([&](const WindowMinimizeEvent& e) {
                m_Minimized = e.minimized;
                return false;
            });

            for (const auto& listener : s_EventListeners) {
                listener(dispatcher);
            }
        }
    }

}
