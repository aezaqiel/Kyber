#pragma once

#include <glm/glm.hpp>

namespace Kyber {

    class Framebuffer
    {
    public:
        Framebuffer() = default;
        Framebuffer(u32 width, u32 height)
        {
            Resize(width, height);
        }

        auto Resize(u32 width, u32 height) -> void
        {
            if (m_Width == width && m_Height == height) return;

            m_Width = width;
            m_Height = height;

            m_Accumulator.assign(width * height, glm::vec3(0.0f));
            m_Image.assign(width * height, glm::vec3(0.0f));
        }

        auto Clear() -> void
        {
            std::fill(m_Accumulator.begin(), m_Accumulator.end(), glm::vec3(0.0f));
            std::fill(m_Image.begin(), m_Image.end(), glm::vec3(0.0f));
        }

        inline auto GetWidth() const -> u32 { return m_Width; }
        inline auto GetHeight() const -> u32 { return m_Height; }

        inline auto GetSize() const -> usize { return m_Accumulator.size(); }

        inline auto GetAccumulatorData() -> std::span<glm::vec3> { return std::span<glm::vec3>(m_Accumulator); }
        inline auto GetImageData() -> std::span<glm::vec3> { return std::span<glm::vec3>(m_Image); }

        inline auto GetAccumulatorData() const -> const std::span<const glm::vec3> { return std::span<const glm::vec3>(m_Accumulator); }
        inline auto GetImageData() const -> const std::span<const glm::vec3> { return std::span<const glm::vec3>(m_Image); }

    private:
        u32 m_Width { 0 };
        u32 m_Height { 0 };

        std::vector<glm::vec3> m_Accumulator;
        std::vector<glm::vec3> m_Image;
    };

}
