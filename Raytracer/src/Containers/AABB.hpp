#pragma once

#include <glm/glm.hpp>

#include "Interval.hpp"
#include "Ray.hpp"

namespace Kyber {

    struct AABB
    {
        Interval x;
        Interval y;
        Interval z;

        AABB() = default;

        AABB(const Interval& x, const Interval& y, const Interval& z)
            : x(x), y(y), z(z)
        {
        }

        AABB(const glm::vec3& a, const glm::vec3& b)
        {
            x = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
            y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
            z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
        }

        AABB(const AABB& b0, const AABB& b1)
        {
            x = Interval(b0.x, b1.x);
            y = Interval(b0.y, b1.y);
            z = Interval(b0.z, b1.z);
        }

        auto AxisInterval(usize index) const -> const Interval&
        {
            if (index == 1) return y;
            if (index == 2) return z;
            return x;
        }

        auto Hit(const Ray& ray, Interval clip) const -> bool;
    };

}
