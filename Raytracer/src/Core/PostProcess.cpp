#include "PostProcess.hpp"

#include <glad/gl.h>

#include <PathConfig.inl>

namespace Kyber {

    namespace {

        std::filesystem::path s_ResPath(PathConfig::ResDir);

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
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Texture);
        glTextureStorage2D(m_Texture, 1, GL_RGBA8, m_Width, m_Height);

        glTextureParameteri(m_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        usize bufferSize = m_Width * m_Height * sizeof(glm::vec4);
        GLbitfield bufferFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

        glCreateBuffers(1, &m_Buffer);
        glNamedBufferStorage(m_Buffer, bufferSize, nullptr, bufferFlags);

        m_MappedBuffer = glMapNamedBufferRange(m_Buffer, 0, bufferSize, bufferFlags);

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
        glUnmapNamedBuffer(m_Buffer);
        glDeleteBuffers(1, &m_Buffer);

        glDeleteTextures(1, &m_Texture);
    }

    auto PostProcess::UploadTiles(const std::span<const glm::vec4>& framebuffer, const std::vector<Tile>& tiles) const -> void
    {
        for (const auto& tile : tiles) {
            usize rowStride = m_Width * sizeof(glm::vec4);
            usize tileRowBytes = tile.w * sizeof(glm::vec4);

            for (u32 row = 0; row < tile.h; ++row) {
                usize pixelIndex = tile.x + (tile.y + row) * m_Width;

                std::memcpy(
                    static_cast<u8*>(m_MappedBuffer) + pixelIndex * sizeof(glm::vec4),
                    framebuffer.data() + pixelIndex,
                    tileRowBytes
                );
            }
        }
    }

    auto PostProcess::Dispatch() const -> void
    {
        glUseProgram(m_Program);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffer);
        glBindImageTexture(0, m_Texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

        glUniform1i(glGetUniformLocation(m_Program, "u_Width"), m_Width);
        glUniform1i(glGetUniformLocation(m_Program, "u_Height"), m_Height);

        glDispatchCompute((m_Width + 15) / 16, (m_Height + 15) / 16, 1);

        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

}
