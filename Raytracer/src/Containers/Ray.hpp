#pragma once

#include <glm/glm.hpp>

namespace Kyber {

    struct Ray
    {
        glm::vec3 origin;
        glm::vec3 direction;
    
        Ray()
            : origin(0.0f), direction(0.0f)
        {
        }

        Ray(const glm::vec3& o, const glm::vec3& d)
            : origin(o), direction(d)
        {
        }

        auto At(f32 t) const -> glm::vec3
        {
            return origin + t * direction;
        }
    };

}
