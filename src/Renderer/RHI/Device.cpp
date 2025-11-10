#include "Device.hpp"

#include "Instance.hpp"

namespace Kyber::Renderer::RHI {

    Device::Device(const std::shared_ptr<Instance>& instance)
        : m_Instance(instance)
    {
        SelectPhysicalDevice();
        CreateDevice();
    }

    Device::~Device()
    {
        vkDeviceWaitIdle(m_Device);
        vmaDestroyAllocator(m_Allocator);
        vkDestroyDevice(m_Device, nullptr);
    }

    void Device::SelectPhysicalDevice()
    {
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance->GetInstance(), &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance->GetInstance(), &deviceCount, availableDevices.data());

        std::multimap<i32, VkPhysicalDevice> scoredDevices;
        for (const auto& device : availableDevices) {
            i32 score = ScorePhysicalDevice(device, m_Instance->GetSurface());
            scoredDevices.insert(std::make_pair(score, device));
        }

        if (scoredDevices.rbegin()->first > 0) {
            m_PhysicalDevice = scoredDevices.rbegin()->second;
            m_QueueFamily = FindQueueFamilyIndices(m_PhysicalDevice, m_Instance->GetSurface());

            m_RTProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
            m_RTProps.pNext = nullptr;

            m_Props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
            m_Props.pNext = &m_RTProps;

            vkGetPhysicalDeviceProperties2(m_PhysicalDevice, &m_Props);
            LOG_INFO("Physical device: {}", m_Props.properties.deviceName);
            LOG_INFO("Graphics queue family: {}", GetGraphicsQueueFamily());
            LOG_INFO("Compute queue family: {}", GetComputeQueueFamily());
            LOG_INFO("Transfer queue family: {}", GetTransferQueueFamily());
        }
    }

    void Device::CreateDevice()
    {
        std::set<u32> uniqueFamily {
            GetGraphicsQueueFamily(),
            GetComputeQueueFamily(),
            GetTransferQueueFamily()
        };

        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        queueInfos.reserve(uniqueFamily.size());

        f32 queuePriority = 1.0f;
        for (u32 index : uniqueFamily) {
            queueInfos.push_back(VkDeviceQueueCreateInfo {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = index,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority
            });
        }

        std::vector<const char*> extensions {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkPhysicalDeviceDynamicRenderingFeatures dynamicRendering {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = nullptr,
            .dynamicRendering = VK_TRUE
        };

        VkPhysicalDeviceSynchronization2Features sync2 {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext = &dynamicRendering,
            .synchronization2 = VK_TRUE
        };

        VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphore {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
            .pNext = &sync2,
            .timelineSemaphore = VK_TRUE
        };

        VkPhysicalDeviceFeatures2 features {
            .sType  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &timelineSemaphore,
            .features = {}
        };

        VkDeviceCreateInfo deviceInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &features,
            .flags = 0,
            .queueCreateInfoCount = static_cast<u32>(queueInfos.size()),
            .pQueueCreateInfos = queueInfos.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<u32>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
            .pEnabledFeatures = nullptr
        };

        VK_CHECK(vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device));
        volkLoadDevice(m_Device);

        VmaAllocatorCreateInfo allocatorInfo {
            .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .physicalDevice = m_PhysicalDevice,
            .device = m_Device,
            .preferredLargeHeapBlockSize = 0,
            .pAllocationCallbacks = nullptr,
            .pDeviceMemoryCallbacks = nullptr,
            .pHeapSizeLimit = nullptr,
            .pVulkanFunctions = nullptr,
            .instance = m_Instance->GetInstance(),
            .vulkanApiVersion = volkGetInstanceVersion()
        };

        VmaVulkanFunctions vmaFuncs;
        vmaImportVulkanFunctionsFromVolk(&allocatorInfo, &vmaFuncs);
        allocatorInfo.pVulkanFunctions = &vmaFuncs;

        VK_CHECK(vmaCreateAllocator(&allocatorInfo, &m_Allocator));

        vkGetDeviceQueue(m_Device, GetGraphicsQueueFamily(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, GetComputeQueueFamily(), 0, &m_ComputeQueue);
        vkGetDeviceQueue(m_Device, GetTransferQueueFamily(), 0, &m_TransferQueue);
    }

    // TODO: Currently only look for discrete gpu
    i32 Device::ScorePhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices = FindQueueFamilyIndices(device, surface);
        if (!indices.IsComplete()) return 0;

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);

        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return 1000;
        }

        return 0;
    }

    Device::QueueFamilyIndices Device::FindQueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices;
        
        u32 familyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
        std::vector<VkQueueFamilyProperties> availableFamilies(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, availableFamilies.data());

        for (u32 i = 0; i < familyCount; ++i) {
            const auto& family = availableFamilies.at(i);

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupport) {
                if (!indices.graphics.has_value()) {
                    indices.graphics = i;
                }
            }

            if ((family.queueFlags & VK_QUEUE_COMPUTE_BIT) && !(family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                if (!indices.compute.has_value()) {
                    indices.compute = i;
                }
            }

            if ((family.queueFlags & VK_QUEUE_TRANSFER_BIT) && !(family.queueFlags & VK_QUEUE_COMPUTE_BIT) && !(family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                if (!indices.transfer.has_value()) {
                    indices.transfer = i;
                }
            }
        }

        for (u32 i = 0; i < familyCount; ++i) {
            const auto& family = availableFamilies.at(i);

            if (!indices.compute.has_value() && (family.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                indices.compute = i;
            }

            if (!indices.transfer.has_value()) {
                if ((family.queueFlags & VK_QUEUE_COMPUTE_BIT) && !(family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                    indices.transfer = i;
                }
            }
        }

        if (!indices.transfer.has_value()) {
            indices.transfer = indices.compute;
        }

        if (!indices.compute.has_value()) {
            indices.compute = indices.graphics;
        }

        if (!indices.transfer.has_value()) {
            indices.transfer = indices.graphics;
        }

        return indices;
    }

}
