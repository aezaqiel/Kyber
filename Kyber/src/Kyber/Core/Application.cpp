#include "Application.hpp"

#include <glad/gl.h>
#include <imgui.h>

#include "Input.hpp"

namespace Kyber {

    Application::Application()
    {
        m_Window = std::make_shared<Window>(1280, 720, "Kyber");
        m_Window->BindEventCallback(BIND_EVENT_FN(Application::DispatchEvents));

        m_ImGuiLayer = CreateOverlay<ImGuiLayer>(m_Window);
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

            // TODO: delta time
            for (auto& layer : m_LayerStack) {
                layer->OnUpdate(0.0f);
            }

            if (!m_Minimized) {
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                m_ImGuiLayer->Begin();
                for (auto& layer : m_LayerStack) {
                    layer->OnImGuiRender();
                }
                m_ImGuiLayer->End();

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

        for (auto& layer : m_LayerStack | std::ranges::views::reverse) {
            if (event.handled) break;
            layer->OnEvent(dispatcher);
        }
    }

}
