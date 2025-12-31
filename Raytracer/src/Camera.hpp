#pragma once

#include "Containers/Ray.hpp"

namespace Kyber {

    class Camera
    {
    public:
        Camera(
            u32 width,
            u32 height,
            f32 vfov,
            const glm::vec3& lookfrom,
            const glm::vec3& lookat,
            const glm::vec3& vup,
            f32 defocusAngle,
            f32 focusDist
        );

        ~Camera() = default;

        auto Resize(u32 width, u32 height) -> void;
        auto GetRay(u32 x, u32 y, const glm::vec2& offset = glm::vec2(0.0f)) const -> Ray;

    private:
        f32 m_VFOV;

        glm::vec3 m_LookFrom;
        glm::vec3 m_LookAt;
        glm::vec3 m_VUp;

        f32 m_DefocusAngle;
        f32 m_FocusDist;

        glm::vec3 m_Pixel00;
        glm::vec3 m_PixelDu;
        glm::vec3 m_PixelDv;

        glm::vec3 m_DefocusU;
        glm::vec3 m_DefocusV;
    };

}
