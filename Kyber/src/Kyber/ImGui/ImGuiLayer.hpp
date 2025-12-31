#pragma once

#include "Kyber/Core/Layer.hpp"
#include "Kyber/Core/Window.hpp"

namespace Kyber {

    class ImGuiLayer final : public Layer
    {
    public:
        ImGuiLayer(const std::shared_ptr<Window>& window);

        virtual ~ImGuiLayer() = default;

        virtual auto OnAttach() -> void override;
        virtual auto OnDetach() -> void override;

        virtual auto OnEvent(const EventDispatcher& dispatcher) -> void override;

        auto Begin() -> void;
        auto End() -> void;

    private:
        std::shared_ptr<Window> m_Window;
    };

}
