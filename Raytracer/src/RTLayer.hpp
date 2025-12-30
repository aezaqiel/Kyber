#pragma once

#include <Kyber/Kyber.hpp>

namespace Raytracer {

    class RTLayer final : public Kyber::Layer
    {
    public:
        RTLayer()
            : Kyber::Layer("RT")
        {
        }

        virtual ~RTLayer() = default;

        virtual auto OnAttach() -> void override;
        virtual auto OnDetach() -> void override;

        virtual auto OnEvent(const Kyber::EventDispatcher& event) -> void override;

        virtual auto OnUpdate(Kyber::f32 dt) -> void override;

    };

}
