#include "Dielectric.hpp"

#include "Core/RNG.hpp"

namespace Kyber {
    
    namespace {

        auto Reflectance(f32 cosine, f32 ri) -> f32
        {
            f32 r0 = ( 1.0f - ri) / (1.0f + ri);
            r0 = r0 * r0;
            return r0 + (1.0f - r0) * glm::pow((1.0f - cosine), 5.0f);
        }

    }

    Dielectric::Dielectric(f32 ri)
        : m_RI(ri)
    {
    }

    auto Dielectric::Scatter(const Ray& ray, const HitRecord& hit) const -> std::optional<ScatterData>
    {
        f32 ri = hit.frontFace ? (1.0f / m_RI) : m_RI;

        glm::vec3 rdir = glm::normalize(ray.direction);

        f32 cosTheta = std::fmin(glm::dot(-rdir, hit.n), 1.0f);
        f32 sinTheta = glm::sqrt(1.0f - cosTheta * cosTheta);

        bool cannotRefract = ri * sinTheta > 1.0f;

        glm::vec3 direction;
        if (cannotRefract || (Reflectance(cosTheta, ri) > RNG::F32())) {
            direction = glm::reflect(rdir, hit.n);
        } else {
            direction = glm::refract(rdir, hit.n, ri);
        }

        return ScatterData {
            .scattered = Ray(hit.p, direction),
            .attenuation = glm::vec3(1.0f)
        };
    }

}
