#include "Raytracer.hpp"

Kyber::Application* Kyber::CreateApplication()
{
    return new Raytracer::Raytracer();
}
