#include "Renderer.hpp"

#include "Core/Window.hpp"

namespace Kyber::Renderer {

    Renderer::Renderer(const std::shared_ptr<Core::Window>& window)
        : m_Window(window)
    {
        m_RenderQueue = std::make_unique<RenderQueue>(s_FrameInFlight);
        m_RenderThread = std::jthread(&Renderer::RenderLoop, this);
    }

    Renderer::~Renderer()
    {
        m_Running.store(false);

        if (m_RenderThread.joinable()) {
            m_RenderThread.join();
        }
    }

    void Renderer::SubmitFrame(RenderPacket&& packet)
    {
        if (!m_Running.load()) {
            return;
        }

        m_RenderQueue->Emplace(std::move(packet));
    }

    void Renderer::RenderLoop()
    {
        LOG_INFO("Render thread spawned");

        RenderPacket packet;
        while (true) {
            if (!m_Running.load()) {
                break;
            }

            m_RenderQueue->Pop(packet);
            DrawFrame(packet);
        }

        LOG_INFO("Render thread shutting down");
    }

    void Renderer::DrawFrame(const RenderPacket& packet)
    {
    }

}
