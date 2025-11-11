#pragma once

#include "VkTypes.hpp"
#include <vk_mem_alloc.h>

namespace Kyber::Renderer::RHI {

    class Instance;

    enum class QueueType : u8
    {
        Graphics,
        Compute,
        Transfer
    };

    class Device
    {
    public:
        Device(const std::shared_ptr<Instance>& instance);
        ~Device();

        void SyncFrame();

        inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        inline VkDevice GetDevice() const { return m_Device; }

        template <QueueType type = QueueType::Graphics>
        inline constexpr VkQueue GetQueue() const
        {
            if constexpr (type == QueueType::Graphics) {
                return m_GraphicsQueue;
            } else if constexpr (type == QueueType::Compute) {
                return m_ComputeQueue;
            } else if constexpr (type == QueueType::Transfer) {
                return m_TransferQueue;
            }

            return m_GraphicsQueue;
        }

        template <QueueType type = QueueType::Graphics>
        inline constexpr u32 GetQueueFamily() const
        {
            if constexpr (type == QueueType::Graphics) {
                return m_QueueFamily.graphics.value();
            } else if constexpr (type == QueueType::Compute) {
                return m_QueueFamily.compute.value();
            } else if constexpr (type == QueueType::Transfer) {
                return m_QueueFamily.transfer.value();
            }

            return m_QueueFamily.graphics.value();
        }

        inline std::set<u32> GetQueueFamilies() const
        {
            return m_QueueFamily.UniqueFamilies();
        }

        inline void WaitIdle() const { vkDeviceWaitIdle(m_Device); }

        inline VkSemaphore GetFrameSemaphore() const { return m_FrameSemaphore; }
        inline u64 GetHostIndex() const { return m_HostFrameIndex; }

        inline u32 GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }
        inline static constexpr u32 GetFrameInFlight() { return s_FrameInFlight; }

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

            inline std::set<u32> UniqueFamilies() const
            {
                return std::set<u32> {
                    graphics.value(),
                    compute.value(),
                    transfer.value()
                };
            }
        };

    private:
        void SelectPhysicalDevice();
        void CreateDevice();
        void CreateSyncPrimitive();

        static i32 ScorePhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
        static QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface);

    private:
        inline static constexpr usize s_FrameInFlight = 3;

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

        u32 m_CurrentFrameIndex = 0;

        VkSemaphore m_FrameSemaphore = VK_NULL_HANDLE;
        u64 m_HostFrameIndex = 0;
        u64 m_LocalFrameIndex = 0;
    };

}
