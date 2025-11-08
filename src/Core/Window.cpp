#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Kyber::Core {

    Window::Window(const Config& config)
    {
        if (s_InstanceCount.fetch_add(1, std::memory_order_relaxed) == 0) {
            glfwSetErrorCallback([](i32 code, const char* desc) {
                LOG_ERROR("GLFW Error {}: {}", code, desc);
            });

            glfwInit();
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(
            static_cast<i32>(config.width), static_cast<i32>(config.height),
            config.title.c_str(),
            nullptr, nullptr
        );

        {
            i32 w, h;
            glfwGetFramebufferSize(m_Window, &w, &h);
            m_Data.width = static_cast<u32>(w);
            m_Data.height = static_cast<u32>(h);
        }

        glfwSetWindowUserPointer(m_Window, &m_Data);

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
            if (data.queue) {
                if (!data.queue->TryPush(WindowClosedEvent())) {
                    LOG_WARN("Event queue full, dropping event");
                }
            }
        });

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, i32 width, i32 height) {
            WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data.width = static_cast<u32>(width);
            data.height = static_cast<u32>(height);

            if (data.queue) {
                if (!data.queue->TryPush(WindowResizedEvent(width, height))) {
                    LOG_WARN("Event queue full, dropping event");
                }
            }
        });

        glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, i32 x, i32 y) {
            WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (data.queue) {
                if (!data.queue->TryPush(WindowMovedEvent(x, y))) {
                    LOG_WARN("Event queue full, dropping event");
                }
            }
        });

        glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, i32 iconified) {
            WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (data.queue) {
                if (!data.queue->TryPush(WindowMinimizeEvent(iconified))) {
                    LOG_WARN("Event queue full, dropping event");
                }
            }
        });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, i32 focused) {
            WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (data.queue) {
                if (!data.queue->TryPush(WindowFocusEvent(focused))) {
                    LOG_WARN("Event queue full, dropping event");
                }
            }
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, i32 key, i32, i32 action, i32) {
            WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (data.queue) {
                switch (action) {
                    case GLFW_PRESS: {
                        if (!data.queue->TryPush(KeyPressedEvent(static_cast<KeyCode>(key), false))) {
                            LOG_WARN("Event queue full, dropping event");
                        }
                    } break;
                    case GLFW_RELEASE: {
                        if (!data.queue->TryPush(KeyReleasedEvent(static_cast<KeyCode>(key)))) {
                            LOG_WARN("Event queue full, dropping event");
                        }
                    } break;
                    case GLFW_REPEAT: {
                        if (!data.queue->TryPush(KeyPressedEvent(static_cast<KeyCode>(key), true))) {
                            LOG_WARN("Event queue full, dropping event");
                        }
                    } break;
                    default:
                        LOG_WARN("Unknown key action {}", action);
                }
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, u32 codepoint) {
            WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (data.queue) {
                if (!data.queue->TryPush(KeyTypedEvent(codepoint))) {
                    LOG_WARN("Event queue full, dropping event");
                }
            }
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, i32 button, i32 action, i32) {
            WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (data.queue) {
                switch (action) {
                    case GLFW_PRESS: {
                        if (!data.queue->TryPush(MouseButtonPressedEvent(static_cast<MouseButton>(button)))) {
                            LOG_WARN("Event queue full, dropping event");
                        }
                    } break;
                    case GLFW_RELEASE: {
                        if (!data.queue->TryPush(MouseButtonReleasedEvent(static_cast<MouseButton>(button)))) {
                            LOG_WARN("Event queue full, dropping event");
                        }
                    } break;
                    default:
                        LOG_WARN("Unknown mouse button action {}", action);
                }
            }
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, f64 x, f64 y) {
            WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (data.queue) {
                if (!data.queue->TryPush(MouseMovedEvent(x, y))) {
                    LOG_WARN("Event queue full, dropping event");
                }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, f64 x, f64 y) {
            WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

            if (data.queue) {
                if (!data.queue->TryPush(MouseScrolledEvent(x, y))) {
                    LOG_WARN("Event queue full, dropping event");
                }
            }
        });

        LOG_INFO("Created Window \"{}\" ({}, {})", GetTitle(), m_Data.width, m_Data.height);
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
        if (s_InstanceCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            glfwTerminate();
        }
    }

    std::string Window::GetTitle() const
    {
        return std::string(glfwGetWindowTitle(m_Window));
    }

    std::vector<const char*> Window::GetRequiredVulkanExtensions()
    {
        u32 count = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);

        return std::vector<const char*>(extensions, extensions + count);
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

}
