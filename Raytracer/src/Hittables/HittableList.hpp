#pragma once

#include "Hittable.hpp"

namespace Kyber {

    class HittableList final : public Hittable
    {
    public:
        HittableList() = default;
        HittableList(const std::span<std::shared_ptr<Hittable>> hittables);

        virtual ~HittableList() = default;

        auto Push(const std::shared_ptr<Hittable>& hittable);
        auto Push(const std::vector<std::shared_ptr<Hittable>>& hittables);

        virtual auto Hit(const Ray& ray, Interval clip) const -> std::optional<HitRecord> override;
        virtual auto GetBBox() const -> AABB override
        {
            return m_BBox;
        }

    private:
        std::vector<std::shared_ptr<Hittable>> m_Hittables;
        AABB m_BBox;
    };

}
