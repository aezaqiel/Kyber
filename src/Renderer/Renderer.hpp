#pragma once

#include "RenderData.hpp"
#include "Scene/SceneData.hpp"

namespace Kyber::Core {

    class Window;

}

namespace Kyber::Renderer {

    class Renderer
    {
    public:
        Renderer(const std::shared_ptr<Core::Window>& window);
        ~Renderer();

        void UploadScene(const Scene::SceneData& scene);
        void StartRenderThread();

        void UpdateCamera(const CameraData& data);
        void RequestResize(u32 width, u32 height);

    private:
        void RenderLoop(std::stop_token stop);
        void DrawFrame();

        void RecreateSwapchain();

    private:
        std::shared_ptr<Core::Window> m_Window;

        std::jthread m_RenderThread;
        std::stop_source m_StopSource;

        u32 m_Width = 0;
        u32 m_Height = 0;
        std::mutex m_ResizeMutex;

        CameraData m_CameraData;
        std::mutex m_CameraMutex;

        std::atomic<bool> m_CameraUpdated = false;
        std::atomic<bool> m_ResizeRequested = false;

        u32 m_CurrentSample = 0;

    private:
        inline static constexpr usize s_FrameInFlight = 3;
    };

}
