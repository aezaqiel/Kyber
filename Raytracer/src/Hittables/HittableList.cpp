#include "HittableList.hpp"

namespace Kyber {

    HittableList::HittableList(const std::span<std::shared_ptr<Hittable>> hittables)
    {
        m_Hittables.assign(hittables.begin(), hittables.end());
    }

    auto HittableList::Push(const std::shared_ptr<Hittable>& hittable)
    {
        m_Hittables.push_back(hittable);
        m_BBox = AABB(m_BBox, hittable->GetBBox());
    }

    auto HittableList::Push(const std::vector<std::shared_ptr<Hittable>>& hittables)
    {
        for (const auto& hittable : hittables) {
            Push(hittable);
        }
    }

    auto HittableList::Hit(const Ray& ray, Interval clip) const -> std::optional<HitRecord>
    {
        std::optional<HitRecord> record;
        bool hitAny = false;

        for (const auto& hittable : m_Hittables) {
            if (auto hit = hittable->Hit(ray, clip)) {
                hitAny = true;
                clip.max = hit->t;
                record = hit;
            }
        }

        return record;
    }

}
