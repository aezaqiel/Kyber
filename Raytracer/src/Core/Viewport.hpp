#pragma once

#include <glm/glm.hpp>

#include "TileScheduler.hpp"
#include "Framebuffer.hpp"

namespace Kyber {

    class Viewport
    {
    public:
        Viewport(u32 width, u32 height);
        ~Viewport();

        auto Resize(u32 width, u32 height) -> void;

        auto Update(const Framebuffer& framebuffer, const std::vector<Tile>& tiles) -> void;
        auto Draw() -> void;

        inline auto GetTextureID() const -> u32
        {
            return m_Texture;
        }

    private:
        auto InitResources() -> void;
        auto DestroyResources() -> void;

    private:
        u32 m_Width { 0 };
        u32 m_Height { 0 };

        u32 m_Texture { 0 };
        u32 m_VAO { 0 };
        u32 m_Program { 0 };

        u32 m_PBO { 0 };
        void* m_Mapped { nullptr };
    };

}
