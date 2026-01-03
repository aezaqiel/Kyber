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
        struct Stats
        {
            u32 TotalHittables { 0 };
            u32 InternalNodes { 0 };
            u32 LeafNodes { 0 };
            u32 TreeDepth { 0 };
        };

    public:
        static auto Create(std::vector<std::shared_ptr<Hittable>>&& primitives) -> std::unique_ptr<BVH>;

        BVH() = default;
        BVH(std::vector<std::shared_ptr<Hittable>>&& primitives, std::vector<LinearBVHNode>&& nodes, const Stats& stats);

        virtual ~BVH() = default;

        virtual AABB GetBBox() const override { return m_Nodes.empty() ? AABB() : m_Nodes[0].bounds; }
        virtual auto Hit(const Ray& ray, Interval clip) const -> std::optional<HitRecord> override;

        auto GetStats() const -> Stats { return m_Stats; }

    private:
        std::vector<std::shared_ptr<Hittable>> m_Hittables;
        std::vector<LinearBVHNode> m_Nodes;

        Stats m_Stats;
    };

}
