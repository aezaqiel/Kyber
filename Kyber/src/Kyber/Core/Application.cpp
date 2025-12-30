#include "Application.hpp"

#include "Input.hpp"

namespace Kyber {

    Application::Application()
    {
        m_Window = std::make_unique<Window>(1280, 720, "Kyber");
        m_Window->BindEventCallback(BIND_EVENT_FN(Application::DispatchEvents));
    }

    Application::~Application()
    {
    }

    auto Application::Run() -> void
    {
        while (m_Running) {
            Window::PollEvents();

            // TEMPORARY
            if (Input::IsKeyDown(KeyCode::Escape)) {
                m_Running = false;
            }

            if (!m_Minimized) {
                m_Window->SwapBuffers();
            }

            Input::Update();
        }
    }

    auto Application::DispatchEvents(const Event& event) -> void
    {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowClosedEvent>([this](const WindowClosedEvent& e) -> bool {
            m_Running = false;
            return true;
        });

        dispatcher.Dispatch<WindowMinimizeEvent>([this](const WindowMinimizeEvent& e) -> bool {
            m_Minimized = e.minimized;
            return false;
        });

        Input::OnEvent(dispatcher);
    }

}
