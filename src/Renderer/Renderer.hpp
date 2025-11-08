#pragma once

#include "RenderPacket.hpp"
#include "DSA/SPSCQueue.hpp"

namespace Kyber::Core {

    class Window;

}

namespace Kyber::Renderer {

    using RenderQueue = DSA::SPSCQueue<RenderPacket>;

    class Renderer
    {
    public:
        Renderer(const std::shared_ptr<Core::Window>& window);
        ~Renderer();

        void SubmitFrame(RenderPacket&& packet);

        inline static constexpr usize GetFrameInFlight()
        {
            return s_FrameInFlight;
        }

    private:
        void RenderLoop();
        void DrawFrame(const RenderPacket& packet);

    private:
        std::shared_ptr<Core::Window> m_Window;

        std::unique_ptr<RenderQueue> m_RenderQueue;
        std::jthread m_RenderThread;

        std::atomic<bool> m_Running = true;

    private:
        inline static constexpr usize s_FrameInFlight = 3;
    };

}
