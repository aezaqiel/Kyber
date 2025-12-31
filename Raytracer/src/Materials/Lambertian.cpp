#include "lambertian.hpp"

#include <glm/gtx/component_wise.hpp>

#include "Core/RNG.hpp"

namespace Kyber {

    Lambertian::Lambertian(const glm::vec3& albedo)
        : m_Albedo(albedo)
    {
    }

    auto Lambertian::Scatter(const Ray& ray, const HitRecord& hit) const -> std::optional<ScatterData>
    {
        glm::vec3 direction = hit.n + RNG::UnitVec3();

        if (glm::compMax(direction) < std::numeric_limits<f32>::epsilon()) {
            direction = hit.n;
        }

        return ScatterData {
            .scattered = Ray(hit.p, direction),
            .attenuation = m_Albedo
        };
    }

}
