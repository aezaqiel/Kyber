#pragma once

#include "Material.hpp"

namespace Kyber {

    class Dielectric final : public Material
    {
    public:
        Dielectric(f32 ri);
        virtual ~Dielectric() = default;

        virtual auto Scatter(const Ray& ray, const HitRecord& hit) const -> std::optional<ScatterData> override;
    
    private:
        f32 m_RI;
    };

}
