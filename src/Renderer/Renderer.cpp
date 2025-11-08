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

        RenderPacket poison;
        poison.shutdown = true;

        m_RenderQueue->Emplace(std::move(poison));

        if (m_RenderThread.joinable()) {
            m_RenderThread.join();
        }
    }

    void Renderer::SubmitFrame(RenderPacket&& packet)
    {
        if (!m_Running.load()) {
            return;
        }

        if (!m_RenderQueue->TryEmplace(std::move(packet))) {
            // LOG_WARN("Render queue full, frame skipped");
        }
    }

    void Renderer::RenderLoop()
    {
        LOG_INFO("Render thread spawned");

        while (m_Running.load(std::memory_order_acquire)) {
            RenderPacket packet = m_RenderQueue->Pop();
            if (packet.shutdown) {
                break;
            }

            DrawFrame(packet);
        }

        LOG_INFO("Render thread shutting down");
    }

    void Renderer::DrawFrame(const RenderPacket& packet)
    {
    }

}
