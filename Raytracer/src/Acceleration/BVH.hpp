#pragma once

#include "Hittables/Hittable.hpp"

namespace Kyber {

    struct LinearBVHNode
    {
        AABB bounds;
        union {
            u32 primitivesOffset;
            u32 secondChildOffset;
        };
        u16 nPrimitives;
        u8 axis;
        u8 pad;
    };

    class BVH final : public Hittable
    {
    public:
        BVH(const std::shared_ptr<Hittable>& left, const std::shared_ptr<Hittable>& right);
        BVH(std::vector<std::shared_ptr<Hittable>>&& primitives, std::vector<LinearBVHNode>&& nodes);
        virtual ~BVH() = default;

        virtual AABB GetBBox() const override { return m_Nodes.empty() ? AABB() : m_Nodes[0].bounds; }
        virtual auto Hit(const Ray& ray, Interval clip) const -> std::optional<HitRecord> override;

        static auto Create(std::vector<std::shared_ptr<Hittable>>&& primitives) -> std::shared_ptr<Hittable>;

    private:
        std::vector<std::shared_ptr<Hittable>> m_Hittables;
        std::vector<LinearBVHNode> m_Nodes;
    };

}
