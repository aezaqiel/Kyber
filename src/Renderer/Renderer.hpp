#pragma once

#include "Core/Events.hpp"
#include "Scene/SceneData.hpp"
#include "DSA/MPMCQueue.hpp"

#include "RenderData.hpp"
#include "RHI/Instance.hpp"
#include "RHI/Device.hpp"
#include "RHI/Swapchain.hpp"
#include "RHI/CommandManager.hpp"

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
        std::jthread m_RenderThread;
        std::stop_source m_StopSource;

        std::unique_ptr<RenderCommandQueue> m_CommandQueue;

        u32 m_Width = 0;
        u32 m_Height = 0;
        u32 m_CurrentSample = 0;

        CameraData m_CameraData;

        std::shared_ptr<RHI::Instance> m_Instance;
        std::shared_ptr<RHI::Device> m_Device;
        std::shared_ptr<RHI::Swapchain> m_Swapchain;

        std::unique_ptr<RHI::CommandManager<RHI::QueueType::Graphics>> m_GraphicsCommand;
        std::unique_ptr<RHI::CommandManager<RHI::QueueType::Compute>> m_ComputeCommand;
        std::unique_ptr<RHI::CommandManager<RHI::QueueType::Transfer>> m_TransferCommand;
    };

}
