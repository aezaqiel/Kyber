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

        inline void WaitIdle() const { vkDeviceWaitIdle(m_Device); }

        inline u32 GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

        template <QueueType type = QueueType::Graphics>
        inline constexpr VkQueue GetQueue() const
        {
            if constexpr (type == QueueType::Compute) {
                return m_ComputeQueue;
            } else if constexpr (type == QueueType::Transfer) {
                return m_TransferQueue;
            }

            return m_GraphicsQueue;
        }

        template <QueueType type = QueueType::Graphics>
        inline constexpr u32 GetQueueFamily() const
        {
            if constexpr (type == QueueType::Compute) {
                return m_QueueFamily.compute.value();
            } else if constexpr (type == QueueType::Transfer) {
                return m_QueueFamily.transfer.value();
            }

            return m_QueueFamily.graphics.value();
        }

        template <QueueType type = QueueType::Graphics>
        inline VkSemaphore GetTimeline()
        {
            if constexpr (type == QueueType::Compute) {
                return m_ComputeTimeline;
            } else if constexpr (type == QueueType::Transfer) {
                return m_TransferTimeline;
            }

            return m_GraphicsTimeline;
        }

        template <QueueType type = QueueType::Graphics>
        inline u64 GetTimelineValue()
        {
            if constexpr (type == QueueType::Compute) {
                return m_ComputeTimelineValue.load();
            } else if constexpr (type == QueueType::Transfer) {
                return m_TransferTimelineValue.load();
            }

            return m_GraphicsTimelineValue.load();
        }

        template <QueueType type = QueueType::Graphics>
        inline u64 IncrementTimeline()
        {
            if constexpr (type == QueueType::Compute) {
                m_ComputeTimelineValue.fetch_add(1);
                return m_ComputeTimelineValue.load();
            } else if constexpr (type == QueueType::Transfer) {
                m_TransferTimelineValue.fetch_add(1);
                return m_TransferTimelineValue.load();
            }

            m_GraphicsTimelineValue.fetch_add(1);
            return m_GraphicsTimelineValue.load();
        }

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

        VkSemaphore m_GraphicsTimeline = VK_NULL_HANDLE;
        VkSemaphore m_ComputeTimeline = VK_NULL_HANDLE;
        VkSemaphore m_TransferTimeline = VK_NULL_HANDLE;

        std::atomic<u64> m_GraphicsTimelineValue = 0;
        std::atomic<u64> m_ComputeTimelineValue = 0;
        std::atomic<u64> m_TransferTimelineValue = 0;

        u64 m_HostFrameIndex = 0;
        u64 m_LocalFrameIndex = 0;
    };

}
