#include "Application.hpp"

namespace Kyber {

    Application::Application(const Config& config)
    {
        m_Window = std::make_unique<Window>(config.window);
        m_Window->BindEventCallback(BIND_EVENT_FN(Application::DispatchEvents));
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
        while (m_Running) {
            Window::PollEvents();

            if (!m_Minimized) {
                m_Window->SwapBuffers();
            }
        }
    }


    void Application::DispatchEvents(const Event& event)
    {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowClosedEvent>([&](const WindowClosedEvent& e) -> bool {
            m_Running = false;

            return true;
        });

        dispatcher.Dispatch<WindowMinimizeEvent>([&](const WindowMinimizeEvent& e) -> bool {
            m_Minimized = e.minimized;
            return false;
        });
    }

}
