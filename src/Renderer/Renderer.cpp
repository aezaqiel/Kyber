#include "Renderer.hpp"

#include "Core/Window.hpp"

namespace Kyber::Renderer {

    Renderer::Renderer(const std::shared_ptr<Core::Window>& window)
        : m_Window(window)
    {
        m_Width = window->GetWidth();
        m_Height = window->GetHeight();
    }

    Renderer::~Renderer()
    {
        if (m_RenderThread.joinable()) {
            m_StopSource.request_stop();
        }
    }

    void Renderer::UploadScene(const Scene::SceneData& scene)
    {
    }

    void Renderer::StartRenderThread()
    {
        if (m_RenderThread.joinable()) {
            LOG_WARN("Render thread already running");
            return;
        }

        LOG_INFO("Spawning render thread");
        m_RenderThread = std::jthread(&Renderer::RenderLoop, this, m_StopSource.get_token());
    }

    void Renderer::UpdateCamera(const CameraData& data)
    {
        std::lock_guard lock(m_CameraMutex);
        m_CameraData = data;
        m_CameraUpdated.store(true, std::memory_order_release);
    }

    void Renderer::RequestResize(u32 width, u32 height)
    {
        std::lock_guard lock(m_ResizeMutex);
        m_Width = width;
        m_Height = height;
        m_ResizeRequested.exchange(true, std::memory_order_release);
    }

    void Renderer::RenderLoop(std::stop_token stop)
    {
        LOG_INFO("Render thread spawned");

        while (!stop.stop_requested()) {
            if (m_ResizeRequested.exchange(false, std::memory_order_acq_rel)) {
                RecreateSwapchain();
                m_CurrentSample = 0;
            }

            if (m_CameraUpdated.exchange(false, std::memory_order_acq_rel)) {
                m_CurrentSample = 0;
            }

            DrawFrame();
            m_CurrentSample++;
        }

        LOG_INFO("Render thread shutting down");
    }

    void Renderer::DrawFrame()
    {
        CameraData camera;
        {
            std::lock_guard lock(m_CameraMutex);
            camera = m_CameraData;
        }

        u32 width, height;
        {
            std::lock_guard lock(m_ResizeMutex);
            width = m_Width;
            height = m_Height;
        }
    }

    void Renderer::RecreateSwapchain()
    {
        LOG_INFO("Renderer resized ({}, {})", m_Width, m_Height);
    }

}
