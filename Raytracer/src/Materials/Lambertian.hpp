#pragma once

#include "Material.hpp"

namespace Kyber {

    class Lambertian final : public Material
    {
    public:
        Lambertian(const glm::vec3& albedo);
        virtual ~Lambertian() = default;

        virtual auto Scatter(const Ray& ray, const HitRecord& hit) const -> std::optional<ScatterData> override;
    
    private:
        glm::vec3 m_Albedo;
    };

}
