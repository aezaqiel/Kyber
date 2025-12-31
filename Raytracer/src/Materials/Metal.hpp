#pragma once

#include "Material.hpp"

namespace Kyber {

    class Metal final : public Material
    {
    public:
        Metal(const glm::vec3& albedo, f32 fuzz);
        virtual ~Metal() = default;

        virtual auto Scatter(const Ray& ray, const HitRecord& hit) const -> std::optional<ScatterData> override;
    
    private:
        glm::vec3 m_Albedo;
        f32 m_Fuzz { 0.0f };
    };

}
