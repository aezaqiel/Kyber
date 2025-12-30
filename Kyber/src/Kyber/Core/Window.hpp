#pragma once

#include "Events.hpp"

struct GLFWwindow;

namespace Kyber {

    class Window
    {
        friend class Application;
        using EventCallbackFn = std::function<void(const Event&)>;
    public:
        Window(u32 width, u32 height, const std::string& title);
        ~Window();

        auto SwapBuffers() -> void;

        inline auto GetTitle() const -> std::string
        {
            return m_Data.title;
        }

        inline auto GetWidth() const -> u32
        {
            return m_Data.width;
        }

        inline auto GetHeight() const -> u32
        {
            return m_Data.height;
        }

        inline auto GetNative() const -> GLFWwindow*
        {
            return m_Window;
        }

        inline auto BindEventCallback(EventCallbackFn&& callback) -> void
        {
            m_Data.callback = std::forward<EventCallbackFn>(callback);
        }

    protected:
        static auto PollEvents() -> void;

    private:
        struct WindowData
        {
            std::string title;
            u32 width { 0 };
            u32 height { 0 };

            EventCallbackFn callback { nullptr };
        };

    private:
        inline static std::atomic<usize> s_Instance { 0 };

        GLFWwindow* m_Window { nullptr };
        WindowData m_Data;
    };

}
