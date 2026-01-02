#include "PostProcess.hpp"

#include <glad/gl.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <PathConfig.inl>

namespace Kyber {

    namespace {

        std::filesystem::path s_ResPath(PathConfig::ResDir);
        std::filesystem::path s_OutPath(PathConfig::OutDir);

        std::string LoadShader(const std::string& filename)
        {
            std::ifstream file(s_ResPath / filename);

            if (!file.is_open()) {
                KERROR("Failed to open shader file: {}", filename);
                return "";
            }

            std::stringstream ss;
            ss << file.rdbuf();

            return ss.str();
        }

        void CheckShader(u32 shader) {
            i32 success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (!success) {
                char info[512]; glGetShaderInfoLog(shader, 512, nullptr, info);
                KERROR("Shader error: {}", info);
            }
        }

        void CheckProgram(u32 program) {
            i32 success;
            glGetProgramiv(program, GL_LINK_STATUS, &success);

            if (!success) {
                char info[512]; glGetProgramInfoLog(program, 512, nullptr, info);
                KERROR("program error: {}", info);
            }
        }

    }

    PostProcess::PostProcess(u32 width, u32 height)
        : m_Width(width), m_Height(height)
    {
        usize bufferSize = m_Width * m_Height * sizeof(glm::vec4);
        GLbitfield bufferFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

        glCreateBuffers(1, &m_StagingBuffer);
        glNamedBufferStorage(m_StagingBuffer, bufferSize, nullptr, bufferFlags);

        m_MappedPtr = glMapNamedBufferRange(m_StagingBuffer, 0, bufferSize, bufferFlags);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_InputTexture);
        glTextureStorage2D(m_InputTexture, 1, GL_RGBA32F, m_Width, m_Height);
        glTextureParameteri(m_InputTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_InputTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_OutputTexture);
        glTextureStorage2D(m_OutputTexture, 1, GL_RGBA8, m_Width, m_Height);
        glTextureParameteri(m_OutputTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_OutputTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::string src = LoadShader("Shaders/PostProcess.comp");
        const char* srcStr = src.c_str();

        u32 cs = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(cs, 1, &srcStr, nullptr);
        glCompileShader(cs);
        CheckShader(cs);

        m_Program = glCreateProgram();
        glAttachShader(m_Program, cs);
        glLinkProgram(m_Program);
        CheckProgram(m_Program);

        glDeleteShader(cs);
    }

    PostProcess::~PostProcess()
    {
        glUnmapNamedBuffer(m_StagingBuffer);
        glDeleteBuffers(1, &m_StagingBuffer);

        glDeleteTextures(1, &m_InputTexture);
        glDeleteTextures(1, &m_OutputTexture);

        glDeleteProgram(m_Program);
    }

    auto PostProcess::UploadTiles(const std::span<const glm::vec4>& framebuffer, const std::vector<Tile>& tiles) const -> void
    {
        std::for_each(std::execution::par, tiles.begin(), tiles.end(), [&](const Tile& tile) {
            usize rowStride = m_Width * sizeof(glm::vec4);
            usize tileRowBytes = tile.w * sizeof(glm::vec4);

            if (tile.w == m_Width) {
                usize startPixel = tile.x + tile.y * m_Width;
                std::memcpy(
                    static_cast<std::byte*>(m_MappedPtr) + startPixel * sizeof(glm::vec4),
                    framebuffer.data() + startPixel,
                    tileRowBytes * tile.h
                );
            } else {
                for (u32 row = 0; row < tile.h; ++row) {
                    usize pixelIndex = tile.x + (tile.y + row) * m_Width;
                    std::memcpy(
                        static_cast<std::byte*>(m_MappedPtr) + pixelIndex * sizeof(glm::vec4),
                        framebuffer.data() + pixelIndex,
                        tileRowBytes
                    );
                }
            }
        });
    }

    auto PostProcess::Dispatch() const -> void
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_StagingBuffer);
        glTextureSubImage2D(m_InputTexture, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_FLOAT, nullptr);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        glUseProgram(m_Program);

        glBindTextureUnit(0, m_InputTexture);
        glBindImageTexture(0, m_OutputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

        glUniform1i(0, m_Width);
        glUniform1i(1, m_Height);

        glDispatchCompute((m_Width + 7) / 8, (m_Height + 7) / 8, 1);

        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    auto PostProcess::Clear() -> void
    {
        std::vector<glm::vec4> clear(m_Width * m_Height, glm::vec4(0.0f));
        std::memcpy(m_MappedPtr, clear.data(), clear.size() * sizeof(glm::vec4));

        Dispatch();
    }

    auto PostProcess::Save(const std::string& filename) -> void
    {
        std::vector<u8> pixels(m_Width * m_Height * 4, 0);
        glGetTextureImage(m_OutputTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.size(), pixels.data());

        if (!std::filesystem::exists(s_OutPath)) {
            std::filesystem::create_directory(s_OutPath);
        }

        auto outfile = (s_OutPath / filename).string();

        stbi_flip_vertically_on_write(true);

        if (stbi_write_png(outfile.c_str(), m_Width, m_Height, 4, pixels.data(), m_Width * 4)) {
            KINFO("Image saved to {}", filename);
        } else {
            KERROR("Failed to save image");
        }
    }

}
