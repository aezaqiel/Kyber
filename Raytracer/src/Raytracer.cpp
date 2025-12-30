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
