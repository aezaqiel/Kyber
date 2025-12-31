#include "Camera.hpp"

#include "Core/RNG.hpp"

namespace Kyber {

    Camera::Camera(
        u32 width,
        u32 height,
        f32 vfov,
        const glm::vec3& lookfrom,
        const glm::vec3& lookat,
        const glm::vec3& vup,
        f32 defocusAngle,
        f32 focusDist
    )
        : m_VFOV(vfov)
        , m_LookFrom(lookfrom)
        , m_LookAt(lookat)
        , m_VUp(vup)
        , m_DefocusAngle(defocusAngle)
        , m_FocusDist(focusDist)
    {
        Resize(width, height);
    }

    auto Camera::Resize(u32 width, u32 height) -> void
    {
        f32 ar = static_cast<f32>(width) / static_cast<f32>(height);

        f32 theta = glm::radians(m_VFOV);
        f32 h = glm::tan(theta / 2.0f);

        f32 viewportHeight = 2.0f * h * m_FocusDist;
        f32 viewportWidth = viewportHeight * ar;

        glm::vec3 w = glm::normalize(m_LookFrom - m_LookAt);
        glm::vec3 u = glm::normalize(glm::cross(m_VUp, w));
        glm::vec3 v = glm::cross(w, u);

        glm::vec3 viewportU = viewportWidth * u;
        glm::vec3 viewportV = viewportHeight * v;
        glm::vec3 viewportW = m_FocusDist * w;

        m_PixelDu = viewportU / static_cast<f32>(width);
        m_PixelDv = viewportV / static_cast<f32>(height);

        glm::vec3 viewportOrigin = m_LookFrom - viewportW - viewportU / 2.0f - viewportV / 2.0f;
        m_Pixel00 = viewportOrigin + 0.5f * (m_PixelDu + m_PixelDv);

        f32 defocusRadius = m_FocusDist * glm::tan(glm::radians(m_DefocusAngle / 2.0f));
        m_DefocusU = u * defocusRadius;
        m_DefocusV = v * defocusRadius;
    }

    auto Camera::GetRay(u32 x, u32 y, const glm::vec2& offset) const -> Ray
    {
        glm::vec3 pixel = m_Pixel00
            + (static_cast<f32>(x) + offset.x) * m_PixelDu
            + (static_cast<f32>(y) + offset.y) * m_PixelDv;

        static auto defocusSample = [](const glm::vec3& center, const glm::vec3& u, const glm::vec3& v) -> glm::vec3
        {
            glm::vec2 p = RNG::InUnitDisk();
            return center + p.x * u + p.y * v;
        };

        glm::vec3 origin;
        if (m_DefocusAngle <= 0.0f) {
            origin = m_LookFrom;
        } else {
            origin = defocusSample(m_LookFrom, m_DefocusU, m_DefocusV);
        }

        return Ray(origin, pixel - origin);
    }

}
