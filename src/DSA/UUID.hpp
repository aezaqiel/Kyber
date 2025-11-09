#pragma once

namespace Kyber::DSA {

    class UUID
    {
    public:
        constexpr UUID() noexcept = default;
        constexpr explicit UUID(const std::array<std::byte, 16>& data) noexcept
            : m_Data(data)
        {
        }

        static constexpr UUID Null() noexcept
        {
            return UUID{};
        }

        [[nodiscard]] static UUID Generate();

        [[nodiscard]] constexpr const std::array<std::byte, 16>& bytes() const noexcept
        {
            return m_Data;
        }

        [[nodiscard]] constexpr bool IsNull() const noexcept;

        [[nodiscard]] std::string ToString() const;

        [[nodiscard]] static constexpr std::optional<UUID> FromString(std::string_view str) noexcept;

        [[nodiscard]] constexpr auto operator<=>(const UUID& other) const noexcept = default;
        [[nodiscard]] constexpr bool operator==(const UUID& other) const noexcept = default;

    private:
        std::array<std::byte, 16> m_Data;
    };

}

namespace std {

    template<>
    struct hash<Kyber::DSA::UUID>
    {
        [[nodiscard]] usize operator()(const Kyber::DSA::UUID& uuid) const noexcept;
    };

}
