#pragma once

#include "Device.hpp"

namespace Kyber::Renderer::RHI {

    template <QueueType TQueue>
    class CommandManager
    {
    public:
        CommandManager(const std::shared_ptr<Device>& device)
            : m_Device(device)
        {
            for (usize i = 0; i < Device::GetFrameInFlight(); ++i) {
                VkCommandPoolCreateInfo poolInfo {
                    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                    .queueFamilyIndex = m_Device->GetQueueFamily<TQueue>()
                };

                VK_CHECK(vkCreateCommandPool(m_Device->GetDevice(), &poolInfo, nullptr, &m_CommandPools[i]));

                VkCommandBufferAllocateInfo allocateInfo {
                    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                    .pNext = nullptr,
                    .commandPool = m_CommandPools[i],
                    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                    .commandBufferCount = 1
                };

                VK_CHECK(vkAllocateCommandBuffers(m_Device->GetDevice(), &allocateInfo, &m_CommandBuffers[i]));
            };
        }

        ~CommandManager()
        {
            for (usize i = 0; i < Device::GetFrameInFlight(); ++i) {
                vkDestroyCommandPool(m_Device->GetDevice(), m_CommandPools[i], nullptr);
            }
        }

        void Record(const std::function<void(VkCommandBuffer)>&& func)
        {
            VK_CHECK(vkResetCommandPool(m_Device->GetDevice(), m_CommandPools[m_Device->GetCurrentFrameIndex()], 0));

            VkCommandBufferBeginInfo beginInfo {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = nullptr
            };

            VK_CHECK(vkBeginCommandBuffer(m_CommandBuffers[m_Device->GetCurrentFrameIndex()], &beginInfo));
            func(m_CommandBuffers[m_Device->GetCurrentFrameIndex()]);
            VK_CHECK(vkEndCommandBuffer(m_CommandBuffers[m_Device->GetCurrentFrameIndex()]));
        }

        u64 Submit(const std::span<VkSemaphoreSubmitInfo>& wait, const std::span<VkSemaphoreSubmitInfo>& signal)
        {
            std::vector<VkSemaphoreSubmitInfo> allSignal(signal.begin(), signal.end());
            allSignal.push_back(VkSemaphoreSubmitInfo {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .pNext = nullptr,
                .semaphore = m_Device->GetTimeline<TQueue>(),
                .value = m_Device->IncrementTimeline<TQueue>(),
                .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                .deviceIndex = 0
            });

            VkCommandBufferSubmitInfo cmdSubmitInfo {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .pNext = nullptr,
                .commandBuffer = m_CommandBuffers[m_Device->GetCurrentFrameIndex()],
                .deviceMask = 0
            };

            VkSubmitInfo2 submitInfo {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                .pNext = nullptr,
                .flags = 0,
                .waitSemaphoreInfoCount = static_cast<u32>(wait.size()),
                .pWaitSemaphoreInfos = wait.data(),
                .commandBufferInfoCount = 1,
                .pCommandBufferInfos = &cmdSubmitInfo,
                .signalSemaphoreInfoCount = static_cast<u32>(allSignal.size()),
                .pSignalSemaphoreInfos = allSignal.data()
            };

            VK_CHECK(vkQueueSubmit2(m_Device->GetQueue<TQueue>(), 1, &submitInfo, VK_NULL_HANDLE));
            return m_Device->GetTimelineValue<TQueue>();
        }

    private:
        std::shared_ptr<Device> m_Device;

        std::array<VkCommandPool, Device::GetFrameInFlight()> m_CommandPools;
        std::array<VkCommandBuffer, Device::GetFrameInFlight()> m_CommandBuffers;
    };

}
