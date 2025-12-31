#pragma once

namespace Kyber {

    struct Interval
    {
        f32 min;
        f32 max;

        Interval()
            : min(std::numeric_limits<f32>::infinity()), max(-std::numeric_limits<f32>::infinity())
        {
        }

        Interval(f32 min, f32 max)
            : min(min), max(max)
        {
        }

        Interval(const Interval& a, const Interval& b)
        {
            min = a.min <= b.min ? a.min : b.min;
            max = a.max >= b.max ? a.max : b.max;
        }

        inline f32 Size() const
        {
            return max - min;
        }

        inline bool Contains(f32 x) const
        {
            return min <= x && x <= max;
        }

        inline bool Surrounds(f32 x) const
        {
            return min < x && x < max;
        }

        inline f32 Clamp(f32 x) const
        {
            return std::clamp(x, min, max);
        }

        Interval Expand(f32 delta) const
        {
            f32 pad = delta / 2.0f;
            return Interval(min - pad, max + pad);
        }
    };

}
