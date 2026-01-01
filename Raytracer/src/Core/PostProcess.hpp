#pragma once

#include <glm/glm.hpp>

#include "TileScheduler.hpp"

namespace Kyber {

    class PostProcess
    {
    public:
        PostProcess(u32 width, u32 height);
        ~PostProcess();

        auto UploadTiles(const std::span<const glm::vec4>& framebuffer, const std::vector<Tile>& tiles) const -> void;
        auto Dispatch() const -> void;

        auto Clear() -> void;

        auto GetTextureID() const -> u32
        {
            return m_OutputTexture;
        }

    private:
        u32 m_Width { 0 };
        u32 m_Height { 0 };

        u32 m_StagingBuffer { 0 };
        void* m_MappedPtr { nullptr };

        u32 m_InputTexture { 0 };
        u32 m_OutputTexture { 0 };

        u32 m_Program { 0 };
    };

}
