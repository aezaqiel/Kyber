#include "BVH.hpp"

namespace Kyber {

    namespace {

        struct BVHBuildNode
        {
            AABB bounds;
            BVHBuildNode* children[2];
            u32 splitAxis;
            u32 firstPrimOffset;
            u32 nPrimitives;

            auto InitLeaf(u32 first, u32 n, const AABB& b) -> void
            {
                firstPrimOffset = first;
                nPrimitives = n;
                bounds = b;
                children[0] = nullptr;
                children[1] = nullptr;
            }

            auto InitInterior(u32 axis, BVHBuildNode* c0, BVHBuildNode* c1) -> void
            {
                children[0] = c0;
                children[1] = c1;
                bounds = AABB(c0->bounds, c1->bounds);
                splitAxis = axis;
                nPrimitives = 0;
            }
        };

        auto GetCentroid(const AABB& bbox) -> glm::vec3
        {
            return glm::vec3(
                (bbox.x.min + bbox.x.max) * 0.5f,
                (bbox.y.min + bbox.y.max) * 0.5f,
                (bbox.z.min + bbox.z.max) * 0.5f
            );
        }

        auto BuildBVHRecursive(
            std::vector<std::shared_ptr<Hittable>>& primitives,
            u32 start, u32 end,
            u32& totalNodes,
            u32& totalLeaves,
            u32 depth, u32& maxDepth
        ) -> BVHBuildNode*
        {
            maxDepth = std::max(maxDepth, depth);
            BVHBuildNode* node = new BVHBuildNode();
            totalNodes++;

            AABB centroidBounds;
            AABB bbox;
            for (u32 i = start; i < end; ++i) {
                AABB pb = primitives[i]->GetBBox();
                glm::vec3 c = GetCentroid(pb);
                centroidBounds = AABB(centroidBounds, AABB(c, c));
                bbox = AABB(bbox, pb);
            }

            u32 nPrimitives = end - start;
            if (nPrimitives == 1) {
                node->InitLeaf(start, nPrimitives, bbox);
                totalLeaves++;
                return node;
            }

            AABB nodeBounds = bbox;

            i32 axis = 0;
            f32 maxExtent = centroidBounds.x.Size();
            if (centroidBounds.y.Size() > maxExtent) {
                axis = 1;
                maxExtent = centroidBounds.y.Size();
            }
            if (centroidBounds.z.Size() > maxExtent) {
                axis = 2;
            }

            u32 mid = (start + end) / 2;
            std::nth_element(primitives.begin() + start, primitives.begin() + mid, primitives.begin() + end,
                [axis](const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) -> bool {
                    glm::vec3 ca = GetCentroid(a->GetBBox());
                    glm::vec3 cb = GetCentroid(b->GetBBox());
                    return ca[axis] < cb[axis];
                }
            );

            node->InitInterior(axis,
                BuildBVHRecursive(primitives, start, mid, totalNodes, totalLeaves, depth + 1, maxDepth),
                BuildBVHRecursive(primitives, mid, end, totalNodes, totalLeaves, depth + 1, maxDepth)
            );

            return node;
        }

        auto FlattenBVHTree(BVHBuildNode* node, std::vector<LinearBVHNode>& nodes) -> u32 
        {
            u32 offset = static_cast<u32>(nodes.size());
            nodes.push_back({});
            LinearBVHNode& linearNode = nodes[offset];

            linearNode.bounds = node->bounds;
            linearNode.nPrimitives = static_cast<u16>(node->nPrimitives);
            linearNode.pad = 0;

            if (node->nPrimitives > 0) {
                linearNode.primitivesOffset = node->firstPrimOffset;
                linearNode.axis = 0;
            } else {
                linearNode.axis = static_cast<u8>(node->splitAxis);
                FlattenBVHTree(node->children[0], nodes);
                linearNode.secondChildOffset = FlattenBVHTree(node->children[1], nodes);
            }

            return offset;
        }

        auto DeleteBVHBuildNode(BVHBuildNode* node) -> void 
        {
            if (node) {
                if (node->children[0]) DeleteBVHBuildNode(node->children[0]);
                if (node->children[1]) DeleteBVHBuildNode(node->children[1]);
                delete node;
            }
        }
    
    }

    BVH::BVH(std::vector<std::shared_ptr<Hittable>>&& primitives, std::vector<LinearBVHNode>&& nodes, const Stats& stats)
        : m_Hittables(std::move(primitives)), m_Nodes(std::move(nodes)), m_Stats(stats)
    {
    }

    auto BVH::Hit(const Ray& ray, Interval clip) const -> std::optional<HitRecord>
    {
        if (m_Nodes.empty()) return std::nullopt;

        bool hitAnything = false;

        const glm::vec3& invDir = 1.0f / ray.direction;
        u32 dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };

        u32 toVisitOffset = 0;
        u32 currentNodeIndex = 0;
        u32 nodesToVisit[64];

        HitRecord record;

        while (true) {
            const LinearBVHNode& node = m_Nodes[currentNodeIndex];

            if (node.bounds.Hit(ray, clip)) {
                if (node.nPrimitives > 0) {
                    for (u32 i = 0; i < node.nPrimitives; ++i) {
                        if (auto hit = m_Hittables[node.primitivesOffset + i]->Hit(ray, clip)) {
                            hitAnything = true;
                            record = *hit;
                            clip.max = record.t;
                        }
                    }
                    if (toVisitOffset == 0) break;
                    currentNodeIndex = nodesToVisit[--toVisitOffset];
                } else {
                    if (dirIsNeg[node.axis]) {
                        nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                        currentNodeIndex = node.secondChildOffset;
                    } else {
                        nodesToVisit[toVisitOffset++] = node.secondChildOffset;
                        currentNodeIndex = currentNodeIndex + 1;
                    }
                }
            } else {
                if (toVisitOffset == 0) break;
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            }
        }

        if (hitAnything) {
            return record;
        } else {
            return std::nullopt;
        }
    }

    auto BVH::Create(std::vector<std::shared_ptr<Hittable>>&& primitives) -> std::unique_ptr<BVH>
    {
        if (primitives.empty()) return nullptr;

        u32 maxDepth = 0;
        u32 totalNodes = 0;
        u32 totalLeaves = 0;

        BVHBuildNode* root = BuildBVHRecursive(primitives, 0, static_cast<u32>(primitives.size()), totalNodes, totalLeaves, 0, maxDepth);

        std::vector<LinearBVHNode> linearNodes;
        linearNodes.reserve(totalNodes);
        FlattenBVHTree(root, linearNodes);

        DeleteBVHBuildNode(root);

        KINFO("BVH Construction Metrics");
        KINFO(" - Total Hittables: {}", primitives.size());
        KINFO(" - Internal Nodes: {}", totalNodes);
        KINFO(" - Leaf Nodes: {}", totalLeaves);
        KINFO(" - Max Tree Depth: {}", maxDepth);

        Stats stats {
            .TotalHittables = static_cast<u32>(primitives.size()),
            .InternalNodes = totalNodes,
            .LeafNodes = totalLeaves,
            .TreeDepth = maxDepth
        };

        return std::make_unique<BVH>(std::move(primitives), std::move(linearNodes), stats);
    }

}
