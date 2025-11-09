#include "UUID.hpp"

#include "PRNG.hpp"

namespace Kyber::DSA {

    struct V7State
    {
        u64 lastMs = 0;
        u64 lastRandB = 0;
    };

    static V7State& GetThreadLocalV7State()
    {
        thread_local V7State state;
        return state;
    }

    template <std::integral T>
    constexpr std::optional<T> HexCharToInt(char c) noexcept
    {
        if (c >= '0' && c <= '9') return static_cast<T>(c - '0');
        if (c >= 'a' && c <= 'f') return static_cast<T>(c - 'a' + 10);
        if (c >= 'A' && c <= 'F') return static_cast<T>(c - 'A' + 10);
        return std::nullopt;
    }

    constexpr std::optional<std::byte> ParseHexByte(char c1, char c2) noexcept
    {
        auto h1 = HexCharToInt<u8>(c1);
        auto h2 = HexCharToInt<u8>(c2);

        if (!h1 || !h2) {
            return std::nullopt;
        }

        return static_cast<std::byte>((*h1 << 4) | *h2);
    }

    UUID UUID::Generate()
    {
        const auto now = std::chrono::steady_clock::now();
        const auto nowMsEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        u64 nowMs = static_cast<u64>(nowMsEpoch.count());

        auto& prng = Xoshiro256::GetThreadLocalPRNG();
        auto& state = GetThreadLocalV7State();

        u64 randA = prng() & 0x0FFF;
        u64 randB;

        if (nowMs > state.lastMs) {
            state.lastMs = nowMs;
            randB = prng() & 0x3FFFFFFFFFFFFFFF;
            state.lastRandB = randB;
        } else {
            nowMs = state.lastMs;
            state.lastRandB++;
            randB = state.lastRandB;
        }

        std::array<std::byte, 16> bytes;

        bytes[0] = static_cast<std::byte>(nowMs >> 40);
        bytes[1] = static_cast<std::byte>(nowMs >> 32);
        bytes[2] = static_cast<std::byte>(nowMs >> 24);
        bytes[3] = static_cast<std::byte>(nowMs >> 16);
        bytes[4] = static_cast<std::byte>(nowMs >> 8);
        bytes[5] = static_cast<std::byte>(nowMs);
        bytes[6] = (std::byte{0x70} | static_cast<std::byte>(randA >> 8));
        bytes[7] = static_cast<std::byte>(randA);
        bytes[8] = (std::byte{0x80} | static_cast<std::byte>(randB >> 56));
        bytes[9] = static_cast<std::byte>(randB >> 48);
        bytes[10] = static_cast<std::byte>(randB >> 40);
        bytes[11] = static_cast<std::byte>(randB >> 32);
        bytes[12] = static_cast<std::byte>(randB >> 24);
        bytes[13] = static_cast<std::byte>(randB >> 16);
        bytes[14] = static_cast<std::byte>(randB >> 8);
        bytes[15] = static_cast<std::byte>(randB);

        return UUID(bytes);
    }

    constexpr bool UUID::IsNull() const noexcept
    {
        for (const auto& b : m_Data) {
            if (b != std::byte{0}) {
                return false;
            }
        }
        return true;
    }

    std::string UUID::ToString() const
    {
        return std::apply(
            [](auto... bytes) {
                return std::format(
                    "{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-"
                    "{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
                    static_cast<u8>(bytes)...
                );
            },
            m_Data
        );
    }

    constexpr std::optional<UUID> UUID::FromString(std::string_view str) noexcept
    {
        if (str.length() != 36) {
            return std::nullopt;
        }

        if (str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-') {
            return std::nullopt;
        }

        std::array<std::byte, 16> bytes;
        usize byteIndex = 0;

        for (usize i = 0; i < str.length(); ++i) {
            if (str[i] == '-') {
                continue;
            }

            if (i + 1 >= str.length()) {
                return std::nullopt;
            }

            auto byte = ParseHexByte(str[i], str[i+1]);
            if (!byte) {
                return std::nullopt;
            }

            bytes[byteIndex] = *byte;
            i++;
        }

        if (byteIndex != 16) {
            return std::nullopt;
        }

        return UUID(bytes);
    }

}

usize std::hash<Kyber::DSA::UUID>::operator()(const Kyber::DSA::UUID& uuid) const noexcept
{
    u64 part1, part2;
    std::memcpy(&part1, uuid.bytes().data(), 8);
    std::memcpy(&part2, uuid.bytes().data() + 8, 8);

    const usize h1 = std::hash<u64>{}(part1);
    const usize h2 = std::hash<u64>{}(part2);

    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}
