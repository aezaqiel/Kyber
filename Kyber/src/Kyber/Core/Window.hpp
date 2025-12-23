#pragma once

struct GLFWwindow;

namespace Kyber {

    class Event;

    class Window
    {
        friend class Application;
        using EventCallbackFn = std::function<void(const Event&)>;
    public:
        struct Config
        {
            u32 width;
            u32 height;
            std::string title;
        };
    
    public:
        Window(const Config& config);
        ~Window();

        void SwapBuffers();

        inline std::string GetTitle() const { return m_Data.title; }
        inline u32 GetWidth() const { return m_Data.width; }
        inline u32 GetHeight() const { return m_Data.height; }

        inline GLFWwindow* GetNative() const { return m_Window; }

        inline void BindEventCallback(EventCallbackFn&& callback)
        {
            m_Data.callback = std::forward<EventCallbackFn>(callback);
        }

    protected:
        static void PollEvents();

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
