#include "Application.hpp"

#include "Input.hpp"
#include "JobSystem.hpp"
#include "Assets/GlTFLoader.hpp"

namespace Kyber::Core {

    Application::Application()
    {
        m_Timer = std::make_unique<Timer>();

        m_EventQueue = std::make_unique<EventQueue>(1024);

        m_Window = std::make_shared<Window>(Window::Config(1280, 720, "Kyber"));
        m_Window->BindEventQueue(m_EventQueue.get());

        m_Renderer = std::make_unique<Renderer::Renderer>(m_Window);

        m_Camera = std::make_unique<Scene::Camera>(
            45.0f,
            static_cast<f32>(m_Window->GetWidth()) / static_cast<f32>(m_Window->GetHeight()),
            0.1f, 1000.0f
        );

        m_Renderer->UpdateCamera(m_Camera->GetCameraData());

        JobSystem::Submit([this] {
            if (auto scene = Assets::GlTFLoader::Load("../assets/Suzanne.glb")) {
                m_Renderer->UpdateScene(std::move(scene.value()));
                m_Renderer->StartRenderThread();
            }
        });
    }

    void Application::Run()
    {
        while (m_Running) {
            m_Timer->Tick();

            Window::PollEvents();
            ProcessEvents();
            Input::Update();

            // NOTE: Maybe we dont want this?
            if (Input::IsKeyDown(KeyCode::Escape)) {
                m_Running = false;
            }

            if (!m_Minimized) {
                if (m_ResizeRequest) {
                    m_Renderer->RequestResize(
                        m_ResizeRequest->first,
                        m_ResizeRequest->second
                    );
                    m_ResizeRequest.reset();
                }

                if (m_Camera->OnUpdate(*m_Timer)) {
                    m_Renderer->UpdateCamera(
                        m_Camera->GetCameraData()
                    );
                }
            }
        }
    }

    void Application::ProcessEvents()
    {
        while (auto event = m_EventQueue->TryPop()) {
            EventDispatcher dispatcher(event.value());

            dispatcher.Dispatch<WindowClosedEvent>([&](const WindowClosedEvent&) {
                m_Running = false;
                return true;
            });

            dispatcher.Dispatch<WindowMinimizeEvent>([&](const WindowMinimizeEvent& e) {
                m_Minimized = e.minimized;
                return false;
            });

            dispatcher.Dispatch<WindowResizedEvent>([&](const WindowResizedEvent& e) {
                m_ResizeRequest = std::make_pair(e.width, e.height);
                return false;
            });

            Input::OnEvent(dispatcher);

            m_Camera->OnEvent(dispatcher);
        }
    }

}
