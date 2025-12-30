#pragma once

#include <Kyber/Kyber.hpp>

namespace Raytracer {

    class Raytracer final : public Kyber::Application
    {
    public:
        Raytracer();
        virtual ~Raytracer();
    };

}
