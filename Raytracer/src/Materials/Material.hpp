#pragma once

#include <glm/glm.hpp>

#include "Containers/Ray.hpp"
#include "Hittables/Hittable.hpp"

namespace Kyber {

    struct ScatterData
    {
        Ray scattered;
        glm::vec3 attenuation;
    };

    class Material
    {
    public:
        virtual ~Material() = default;
        virtual auto Scatter(const Ray& ray, const HitRecord& hit) const -> std::optional<ScatterData> = 0;
    };

}
