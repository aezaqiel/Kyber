#include "Viewport.hpp"

#include <glad/gl.h>

#include "PathConfig.inl"

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

    Viewport::Viewport(u32 width, u32 height)
        : m_Width(width), m_Height(height)
    {
        InitResources();
    }

    Viewport::~Viewport()
    {
        DestroyResources();

        glDeleteVertexArrays(1, &m_VAO);
        glDeleteProgram(m_Program);
    }

    auto Viewport::Resize(u32 width, u32 height) -> void
    {
        if (m_Width == width && m_Height == height) return;

        m_Width = width;
        m_Height = height;

        DestroyResources();
        InitResources();
    }

    auto Viewport::Update(const Framebuffer& framebuffer, const std::vector<Tile>& tiles) -> void
    {
        if (tiles.empty()) return;

        const auto& pixels = framebuffer.GetImageData();
        u8* dst = static_cast<u8*>(m_Mapped);

        for (const auto& tile : tiles) {
            usize rowStride = m_Width * sizeof(glm::vec3);
            usize tileRowBytes = tile.w * sizeof(glm::vec3);

            for (u32 row = 0; row < tile.h; ++row) {
                usize pixelIndex = tile.x + (tile.y + row) * m_Width;

                std::memcpy(dst + pixelIndex * sizeof(glm::vec3), pixels.data() + pixelIndex, tileRowBytes);
            }
        }

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBO);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, m_Width);

        for (const auto& tile : tiles) {
            usize offset = (tile.x + tile.y * m_Width) * sizeof(glm::vec3);
            glTextureSubImage2D(m_Texture, 0, tile.x, tile.y, tile.w, tile.h, GL_RGB, GL_FLOAT, reinterpret_cast<void*>(offset));
        }

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }

    auto Viewport::InitResources() -> void
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Texture);
        glTextureStorage2D(m_Texture, 1, GL_RGB32F, m_Width, m_Height);

        glTextureParameteri(m_Texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_Texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        usize buffer_size = m_Width * m_Height * sizeof(glm::vec3);
        glCreateBuffers(1, &m_PBO);

        GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        glNamedBufferStorage(m_PBO, buffer_size, nullptr, flags);

        m_Mapped = glMapNamedBufferRange(m_PBO, 0, buffer_size, flags);

        if (!m_VAO) {
            glCreateVertexArrays(1, &m_VAO);
        }

        if (!m_Program) {
            std::string vs_src = LoadShader("Shaders/Viewport.vert");
            const char* vs_str = vs_src.c_str();

            std::string fs_src = LoadShader("Shaders/Viewport.frag");
            const char* fs_str = fs_src.c_str();

            u32 vs = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vs, 1, &vs_str, nullptr);
            glCompileShader(vs);
            CheckShader(vs);

            u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fs, 1, &fs_str, nullptr);
            glCompileShader(fs);
            CheckShader(fs);

            m_Program = glCreateProgram();
            glAttachShader(m_Program, vs);
            glAttachShader(m_Program, fs);
            glLinkProgram(m_Program);
            CheckProgram(m_Program);

            glDeleteShader(vs);
            glDeleteShader(fs);
        }
    }

    auto Viewport::DestroyResources() -> void
    {
        if (m_PBO) {
            glUnmapNamedBuffer(m_PBO);
            glDeleteBuffers(1, &m_PBO);
        }

        if (m_Texture) glDeleteTextures(1, &m_Texture);
    }

    auto Viewport::Draw() -> void
    {
        glUseProgram(m_Program);
        
        glBindTextureUnit(0, m_Texture);

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

}
