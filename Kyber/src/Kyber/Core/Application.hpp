#pragma once

#include "Window.hpp"
#include "LayerStack.hpp"

#include "Kyber/ImGui/ImGuiLayer.hpp"

namespace Kyber {

    class Application
    {
    public:
        Application();
        virtual ~Application();

        auto Run() -> void;

        template <IsLayer TLayer>
        auto PushLayer(const std::shared_ptr<TLayer>& layer) -> void
        {
            m_LayerStack.PushLayer(layer);
        }

        template <IsLayer TLayer>
        auto PushOverlay(const std::shared_ptr<TLayer>& overlay) -> void
        {
            m_LayerStack.PushOverlay(overlay);
        }

        template <IsLayer TLayer, typename... Args>
            requires std::is_constructible_v<TLayer, Args...>
        auto CreateLayer(Args&&... args) -> std::shared_ptr<TLayer>
        {
            auto layer = std::make_shared<TLayer>(std::forward<Args>(args)...);
            m_LayerStack.PushLayer(layer);
            return layer;
        }

        template <IsLayer TLayer, typename... Args>
            requires std::is_constructible_v<TLayer, Args...>
        auto CreateOverlay(Args&&... args) -> std::shared_ptr<TLayer>
        {
            auto overlay = std::make_shared<TLayer>(std::forward<Args>(args)...);
            m_LayerStack.PushOverlay(overlay);
            return overlay;
        }

    private:
        auto DispatchEvents(const Event& event) -> void;

    private:
        bool m_Running { true };
        bool m_Minimized { false };

        LayerStack m_LayerStack;

        std::shared_ptr<Window> m_Window;
        std::shared_ptr<ImGuiLayer> m_ImGuiLayer;
    };

    Application* CreateApplication();

}
