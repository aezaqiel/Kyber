#pragma once

#include <glm/glm.hpp>

#include "Containers/Ray.hpp"
#include "Containers/Interval.hpp"

namespace Kyber {

    class Material;

    struct HitRecord
    {
        glm::vec3 p;
        glm::vec3 n;
        f32 t;

        bool frontFace;
        auto SetFaceNormal(const Ray& ray, const glm::vec3& normal) -> void
        {
            frontFace = glm::dot(ray.direction, normal) < 0.0f;
            n = frontFace ? normal : -normal;
        }

        std::shared_ptr<Material> material;
    };

    class Hittable
    {
    public:
        Hittable(const std::shared_ptr<Material>& material)
            : m_Material(material)
        {
        }

        virtual ~Hittable() = default;

        virtual auto Hit(const Ray& ray, Interval clip) const -> std::optional<HitRecord> = 0;
    
    protected:
        std::shared_ptr<Material> m_Material;
    };

}
