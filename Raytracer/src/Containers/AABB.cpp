#include "AABB.hpp"

namespace Kyber {

    bool AABB::Hit(const Ray& ray, Interval clip) const
    {
        for (usize axis = 0; axis < 3; ++axis) {
            const Interval& ax = AxisInterval(axis);
            const f32 adinv = 1.0f / ray.direction[axis];

            f32 t0 = (ax.min - ray.origin[axis]) * adinv;
            f32 t1 = (ax.max - ray.origin[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > clip.min) clip.min = t0;
                if (t1 < clip.max) clip.max = t1;
            } else {
                if (t1 > clip.min) clip.min = t1;
                if (t0 < clip.max) clip.max = t0;
            }

            if (clip.max <= clip.min) {
                return false;
            }
        }

        return true;
    }

}
