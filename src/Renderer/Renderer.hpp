#pragma once

#include "RenderData.hpp"

#include "Core/Events.hpp"
#include "Scene/SceneData.hpp"
#include "DSA/MPMCQueue.hpp"

namespace Kyber::Core {

    class Window;

}

namespace Kyber::Renderer {

    class Renderer
    {
    public:
        using RenderCommand = std::function<void(Renderer&)>;
        using RenderCommandQueue = DSA::MPMCQueue<RenderCommand>;

    public:
        Renderer(const std::shared_ptr<Core::Window>& window);
        ~Renderer();

        void StartRenderThread();

        void OnEvent(Core::EventDispatcher& dispatcher);

        void UpdateScene(Scene::SceneData&& scene);
        void UpdateCamera(const CameraData& data);
        void RequestResize(u32 width, u32 height);

    private:
        void RenderLoop(std::stop_token stop);
        void DrawFrame();

        void Submit(RenderCommand&& cmd);
        void ProcessCommands();

        void RecreateSwapchain();
        void UploadScene(Scene::SceneData&& scene);

    private:
        std::shared_ptr<Core::Window> m_Window;

        std::jthread m_RenderThread;
        std::stop_source m_StopSource;

        std::unique_ptr<RenderCommandQueue> m_CommandQueue;

        u32 m_Width = 0;
        u32 m_Height = 0;
        u32 m_CurrentSample = 0;

        CameraData m_CameraData;

    private:
        inline static constexpr usize s_FrameInFlight = 3;
    };

}
