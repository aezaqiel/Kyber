#pragma once

#include <glm/glm.hpp>

#include "Hittable.hpp"

namespace Kyber {

    class Sphere final : public Hittable
    {
    public:
        Sphere(const glm::vec3& center, f32 radius, const std::shared_ptr<Material>& material);
        virtual ~Sphere() = default;

        virtual auto Hit(const Ray& ray, Interval clip) const -> std::optional<HitRecord> override;

    private:
        glm::vec3 m_Center;
        f32 m_Radius { 0 };
    };

}
