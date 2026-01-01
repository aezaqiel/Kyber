#pragma once

#include <glm/glm.hpp>

#include "TileScheduler.hpp"

namespace Kyber {

    class PostProcess
    {
    public:
        PostProcess(u32 width, u32 height);
        ~PostProcess();

        auto Process(const std::span<const glm::vec4>& framebuffer) -> void;

        auto GetTextureID() const -> u32
        {
            return m_Texture;
        }

    private:
        u32 m_Width { 0 };
        u32 m_Height { 0 };

        u32 m_Buffer { 0 };
        void* m_MappedBuffer { nullptr };

        u32 m_Texture { 0 };

        u32 m_Program { 0 };
    };

}
