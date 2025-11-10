#pragma once

#include "VkTypes.hpp"
#include <vk_mem_alloc.h>

namespace Kyber::Renderer::RHI {

    class Instance;

    class Device
    {
    public:
        Device(const std::shared_ptr<Instance>& instance);
        ~Device();

        inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        inline VkDevice GetDevice() const { return m_Device; }

        inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        inline VkQueue GetComputeQueue() const { return m_ComputeQueue; }
        inline VkQueue GetTransferQueue() const { return m_TransferQueue; }

        inline u32 GetGraphicsQueueFamily() const { return m_QueueFamily.Graphics(); }
        inline u32 GetComputeQueueFamily() const { return m_QueueFamily.Compute(); }
        inline u32 GetTransferQueueFamily() const { return m_QueueFamily.Transfer(); }

        inline void WaitIdle() const { vkDeviceWaitIdle(m_Device); }

    private:
        struct QueueFamilyIndices
        {
            std::optional<u32> graphics;
            std::optional<u32> compute;
            std::optional<u32> transfer;

            inline bool IsComplete() const
            {
                return graphics.has_value() && compute.has_value() && transfer.has_value();
            }

            inline u32 Graphics() const
            {
                return graphics.value();
            }

            inline u32 Compute() const
            {
                return compute.value();
            }

            inline u32 Transfer() const
            {
                return transfer.value();
            }
        };

    private:
        void SelectPhysicalDevice();
        void CreateDevice();

        static i32 ScorePhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
        static QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface);

    private:
        std::shared_ptr<Instance> m_Instance;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VmaAllocator m_Allocator = VK_NULL_HANDLE;

        VkPhysicalDeviceProperties2 m_Props;
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_RTProps;

        QueueFamilyIndices m_QueueFamily;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_ComputeQueue = VK_NULL_HANDLE;
        VkQueue m_TransferQueue = VK_NULL_HANDLE;
    };

}
