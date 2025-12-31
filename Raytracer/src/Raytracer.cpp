#include "Raytracer.hpp"

#include "RTLayer.hpp"

namespace Kyber {

    Raytracer::Raytracer()
    {
        CreateLayer<RTLayer>();
    }

    Raytracer::~Raytracer()
    {
    }
    
}

Kyber::Application* Kyber::CreateApplication()
{
    return new Kyber::Raytracer();
}
