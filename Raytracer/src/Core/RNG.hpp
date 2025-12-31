#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Kyber {

    class RNG
    {
    public:
        inline static auto U32() -> u32
        {
            return Next();
        }

        inline static auto U32(u32 min, u32 max) -> u32
        {
            return min + (Next() % (max - min + 1));
        }

        inline static auto F32() -> f32
        {
            u32 r = Next();
            u32 bits = (0x3F800000 | (r >> 9)); 
            return std::bit_cast<f32>(bits) - 1.0f;
        }

        inline static auto F32(f32 min, f32 max) -> f32
        {
            return min + (max - min) * F32();
        }

        inline static auto Vec3() -> glm::vec3
        {
            return glm::vec3(F32(), F32(), F32());
        }

        inline static auto Vec3(f32 min, f32 max) -> glm::vec3
        {
            return glm::vec3(F32(min, max), F32(min, max), F32(min, max));
        }

        inline static auto UnitVec3() -> glm::vec3
        {
            f32 z = F32() * 2.0f - 1.0f;
            f32 a = F32() * 2.0f * glm::pi<f32>();
            f32 r = std::sqrt(1.0f - z * z);
            
            f32 x = r * std::cos(a);
            f32 y = r * std::sin(a);
            
            return glm::vec3(x, y, z);
        }

        inline static auto Vec2() -> glm::vec2
        {
            return glm::vec2(RNG::F32(), RNG::F32());
        }

        inline static auto Vec2(f32 min, f32 max) -> glm::vec2
        {
            return glm::vec2(RNG::F32(min, max), RNG::F32(min, max));
        }

        inline static auto InUnitSphere() -> glm::vec3
        {
            glm::vec3 p;
            do { p = RNG::Vec3(-1.0f, 1.0f); } while (glm::dot(p, p) > 1.0f);
            return p;
        }

        inline static auto InUnitDisk() -> glm::vec2
        {
            glm::vec2 p;
            do { p = RNG::Vec2(-1.0f, 1.0f); } while (glm::dot(p, p) > 1.0f);
            return p;
        }

    private:
        struct XoshiroState
        {
            u32 s[4];

            XoshiroState()
            {
                std::random_device rd;
                u64 seed = (static_cast<u64>(rd()) << 32) | rd(); 

                auto splitmix64 = [&seed]() -> u32 {
                    u64 z = (seed += 0x9e3779b97f4a7c15);
                    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
                    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
                    return static_cast<u32>((z ^ (z >> 31)));
                };

                s[0] = splitmix64();
                s[1] = splitmix64();
                s[2] = splitmix64();
                s[3] = splitmix64();
            }
        };

        inline static auto Next() -> u32
        {
            u32* s = s_State.s;

            const u32 result = std::rotl(s[0] + s[3], 7) + s[0];
            const u32 t = s[1] << 9;

            s[2] ^= s[0];
            s[3] ^= s[1];
            s[1] ^= s[2];
            s[0] ^= s[3];

            s[2] ^= t;
            s[3] = std::rotl(s[3], 11);

            return result;
        }

        inline static thread_local XoshiroState s_State;
    };

}
