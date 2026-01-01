#include "Metal.hpp"

#include "Core/RNG.hpp"

namespace Kyber {

    Metal::Metal(const glm::vec3& albedo, f32 fuzz)
        : m_Albedo(albedo), m_Fuzz(std::max(0.0f, std::min(1.0f, fuzz)))
    {
    }

    auto Metal::Scatter(const Ray& ray, const HitRecord& hit) const -> std::optional<ScatterData>
    {
        glm::vec3 reflected = glm::reflect(glm::normalize(ray.direction), hit.n);
        reflected = glm::normalize(reflected) + (m_Fuzz * RNG::InUnitSphere());

        if (glm::dot(reflected, hit.n) < 0.0f) return std::nullopt;

        return ScatterData {
            .scattered = Ray(hit.p, reflected),
            .attenuation = m_Albedo
        };
    }

}
