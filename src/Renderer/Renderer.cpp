#include "Renderer.hpp"

#include "Core/Window.hpp"

#include "RHI/Instance.hpp"
#include "RHI/Device.hpp"

namespace Kyber::Renderer {

    Renderer::Renderer(const std::shared_ptr<Core::Window>& window)
    {
        m_CommandQueue = std::make_unique<RenderCommandQueue>(1024);

        m_Width = window->GetWidth();
        m_Height = window->GetHeight();

        m_Instance = std::make_shared<RHI::Instance>(window);
        m_Device = std::make_shared<RHI::Device>(m_Instance);
    }

    Renderer::~Renderer()
    {
        if (m_RenderThread.joinable()) {
            m_StopSource.request_stop();
        }
    }

    void Renderer::StartRenderThread()
    {
        if (m_RenderThread.joinable()) {
            LOG_WARN("Render thread already started");
            return;
        }

        LOG_INFO("Spawning render thread");
        m_RenderThread = std::jthread(&Renderer::RenderLoop, this, m_StopSource.get_token());
    }

    void Renderer::UpdateScene(Scene::SceneData&& scene)
    {
        Submit([scene = std::move(scene)](Renderer& renderer) mutable {
            renderer.UploadScene(std::move(scene));
        });
    }

    void Renderer::UpdateCamera(const CameraData& data)
    {
        Submit([data](Renderer& renderer) {
            renderer.m_CameraData = data;
            renderer.m_CurrentSample = 0;
        });
    }

    void Renderer::RequestResize(u32 width, u32 height)
    {
        Submit([width, height](Renderer& renderer) {
            renderer.m_Width = width;
            renderer.m_Height = height;
            renderer.RecreateSwapchain();
            renderer.m_CurrentSample = 0;
        });
    }

    void Renderer::RenderLoop(std::stop_token stop)
    {
        LOG_INFO("Render thread spawned");

        while (!stop.stop_requested()) {
            ProcessCommands();
            DrawFrame();
            m_CurrentSample++;
        }

        LOG_INFO("Render thread shutting down");
    }

    void Renderer::DrawFrame()
    {
    }

    void Renderer::Submit(RenderCommand&& cmd)
    {
        m_CommandQueue->Emplace(std::move(cmd));
    }

    void Renderer::ProcessCommands()
    {
        while (auto cmd = m_CommandQueue->TryPop()) {
            (cmd.value())(*this);
        }
    }

    void Renderer::RecreateSwapchain()
    {
        LOG_INFO("Renderer resized ({}, {})", m_Width, m_Height);
    }

    void Renderer::UploadScene(Scene::SceneData&& scene)
    {
        LOG_INFO("Renderer uploading scene");
    }

}
