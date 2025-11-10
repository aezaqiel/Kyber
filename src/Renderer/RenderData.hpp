#pragma once

#include <glm/glm.hpp>

namespace Kyber::Renderer {

    struct CameraData
    {
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec3 position;
    };

}
