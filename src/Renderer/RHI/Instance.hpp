#pragma once

#include "VkTypes.hpp"

namespace Kyber::Core {

    class Window;

}

namespace Kyber::Renderer::RHI {

    class Instance
    {
    public:
        Instance(const std::shared_ptr<Core::Window>& window);
        ~Instance();

        Instance(const Instance&) = delete;
        Instance& operator=(const Instance&) = delete;

        Instance(Instance&&) = default;
        Instance& operator=(Instance&&) = default;

        inline VkInstance GetInstance() const { return s_Instance; }
        inline VkSurfaceKHR GetSurface() const { return m_Surface; }

    private:
        void CreateInstance();
        void DestroyInstance();

    private:
        std::shared_ptr<Core::Window> m_Window;

        inline static std::atomic<usize> s_InstanceCount = 0;
        inline static VkInstance s_Instance = VK_NULL_HANDLE;
        inline static VkDebugUtilsMessengerEXT s_DebugMessenger = VK_NULL_HANDLE;

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    };

}
