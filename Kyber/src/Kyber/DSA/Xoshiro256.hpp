#pragma once

namespace Kyber {

    class Xoshiro256
    {
    public:
        explicit Xoshiro256(u64 seed)
        {
            for (i32 i = 0; i < 4; ++i) {
                seed += 0x9E3779B97F4A7C15;
                u64 z = seed;
                z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9;
                z = (z ^ (z >> 27)) * 0x94D049BB133111EB;
                s[i] = z ^ (z >> 31);
            }
        }

        u64 operator()()
        {
            const uint64_t result = (s[1] * 5) << 7 | (s[1] * 5) >> (64 - 7); // rotl(s[1] * 5, 7)
            const uint64_t t = s[1] << 17;
            s[2] ^= s[0];
            s[3] ^= s[1];
            s[1] ^= s[2];
            s[0] ^= s[3];
            s[2] ^= t;
            s[3] = (s[3] << 45) | (s[3] >> (64 - 45)); // rotl(s[3], 45)
            return result;
        }

        static constexpr u64 Min() { return 0; }
        static constexpr u64 Max() { return std::numeric_limits<u64>::max(); }

        inline static u64 GetThreadLocalSeed()
        {
            return static_cast<u64>(std::random_device()()) << 32 | std::random_device()();
        }

        inline static Xoshiro256& GetThreadLocalPRNG()
        {
            thread_local Xoshiro256 prng(GetThreadLocalSeed());
            return prng;
        }

    private:
        std::array<u64, 4> s;
    };

}
