#include "Renderer.hpp"

#include "Core/Window.hpp"

namespace Kyber::Renderer {

    Renderer::Renderer(const std::shared_ptr<Core::Window>& window)
    {
        m_CommandQueue = std::make_unique<RenderCommandQueue>(1024);

        m_Width = window->GetWidth();
        m_Height = window->GetHeight();

        m_Instance = std::make_shared<RHI::Instance>(window);
        m_Device = std::make_shared<RHI::Device>(m_Instance);

        m_Swapchain = std::make_shared<RHI::Swapchain>(m_Instance, m_Device);
        m_Swapchain->Create(window->GetWidth(), window->GetHeight());

        m_GraphicsCommand = std::make_unique<RHI::CommandManager<RHI::QueueType::Graphics>>(m_Device);
        m_ComputeCommand = std::make_unique<RHI::CommandManager<RHI::QueueType::Compute>>(m_Device);
        m_TransferCommand = std::make_unique<RHI::CommandManager<RHI::QueueType::Transfer>>(m_Device);
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

        LOG_INFO("Renderer stop requested, waiting for device");
        m_Device->WaitIdle();

        LOG_INFO("Render thread shutdown");
    }

    void Renderer::DrawFrame()
    {
        m_Device->SyncFrame();

        if (m_Swapchain->AcquireNextImage()) {
            RecreateSwapchain();
            return;
        }

        m_GraphicsCommand->Record([&](VkCommandBuffer cmd) {
            VkImageMemoryBarrier barrier {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = VK_ACCESS_NONE,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = m_Swapchain->GetCurrentImage(),
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };

            vkCmdPipelineBarrier(cmd,
                VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );
        });

        std::vector<VkSemaphoreSubmitInfo> wait {
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .pNext = nullptr,
                .semaphore = m_Swapchain->GetCurrentImageSemaphore(),
                .value = 0,
                .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .deviceIndex = 0
            }
        };

        std::vector<VkSemaphoreSubmitInfo> signal {
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .pNext = nullptr,
                .semaphore = m_Device->GetFrameSemaphore(),
                .value = m_Device->GetHostIndex(),
                .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                .deviceIndex = 0
            },
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .pNext = nullptr,
                .semaphore = m_Swapchain->GetPresentSignalSemaphore(),
                .value = 0,
                .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                .deviceIndex = 0
            }
        };

        m_GraphicsCommand->Submit(wait, signal);

        if (m_Swapchain->Present()) {
            RecreateSwapchain();
            return;
        }
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
        LOG_INFO("Swapchain recreate ({}, {})", m_Width, m_Height);

        m_Device->WaitIdle();
        m_Swapchain->Create(m_Width, m_Height);
    }

    void Renderer::UploadScene(Scene::SceneData&& scene)
    {
        LOG_INFO("Renderer uploading scene");
    }

}
