#include "sphere.hpp"

namespace Kyber {

    Sphere::Sphere(const glm::vec3& center, f32 radius, const std::shared_ptr<Material>& material)
        : Hittable(material), m_Center(center), m_Radius(std::max(radius, 0.0f))
    {
    }

    auto Sphere::Hit(const Ray& ray, Interval clip) const -> std::optional<HitRecord>
    {
        glm::vec3 oc = ray.origin - m_Center;

        f32 a = glm::dot(ray.direction, ray.direction);
        f32 b = 2.0f * glm::dot(oc, ray.direction);
        f32 c = glm::dot(oc, oc) - m_Radius * m_Radius;

        f32 discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f) return std::nullopt;

        f32 sqrtd = glm::sqrt(discriminant);
        f32 root = (-b - sqrtd) / (2.0f * a);

        if (!clip.Surrounds(root)) {
            root = (-b + sqrtd) / (2.0f * a);
            if (!clip.Surrounds(root)) return std::nullopt;
        }

        HitRecord record;
        record.t = root;
        record.p = ray.At(root);
        record.SetFaceNormal(ray, (record.p - m_Center) / m_Radius);
        record.material = m_Material;

        return record;
    }

}
