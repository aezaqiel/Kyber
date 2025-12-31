#pragma once

#include "Events.hpp"

namespace Kyber {

    class Layer
    {
    public:
        Layer(const std::string& name = "Layer")
            : m_Name(name)
        {
        }
        virtual ~Layer() = default;

        virtual auto OnAttach() -> void {}
        virtual auto OnDetach() -> void {}

        virtual auto OnEvent(const EventDispatcher& event) -> void {}

        virtual auto OnUpdate(f32 dt) -> void {}

        virtual auto OnImGuiRender() -> void {}

    private:
        std::string m_Name;
    };

    template <typename T>
    concept IsLayer = std::is_base_of_v<Layer, T>;

}
