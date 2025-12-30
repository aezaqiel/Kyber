#include "Raytracer.hpp"

#include "RTLayer.hpp"

namespace Raytracer {

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
    return new Raytracer::Raytracer();
}
