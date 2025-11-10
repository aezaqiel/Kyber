#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Core/Timer.hpp"
#include "Core/Events.hpp"

#include "Renderer/RenderData.hpp"

namespace Kyber::Scene {

    class Camera
    {
    public:
        Camera(f32 fov, f32 aspect, f32 near, f32 far);
        ~Camera() = default;

        bool OnUpdate(const Core::Timer& timer);
        void OnEvent(Core::EventDispatcher& dispatcher);

        void SetViewportSize(f32 width, f32 height);

        inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        inline const glm::mat4& GetProjMatrix() const { return m_ProjMatrix; }
        inline const glm::vec3& GetPosition() const { return m_Position; }

        inline Renderer::CameraData GetCameraData() const
        {
            return std::move(Renderer::CameraData {
                .view = m_ViewMatrix,
                .proj = m_ProjMatrix,
                .position = m_Position
            });
        }

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();
        void UpdateCameraVectors();

    private:
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        glm::mat4 m_ProjMatrix = glm::mat4(1.0f);

        inline static constexpr glm::vec3 s_BaseForward = glm::vec3(0.0f, 0.0f, -1.0f);
        inline static constexpr glm::vec3 s_BaseRight = glm::vec3(1.0f, 0.0f, 0.0f);

        glm::quat m_Orientation = { glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) };

        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, -2.0f);
        glm::vec3 m_Forward = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        f32 m_FOV;
        f32 m_AspectRatio;
        f32 m_NearClip;
        f32 m_FarClip;

        f32 m_MoveSpeed = 5.0f;
        f32 m_MouseSens = 0.1f;
        f32 m_ZoomSpeed = 2.0f;

        glm::vec2 m_LastMousePos = glm::vec2(0.0f, 0.0f);
        f32 m_ScrollDelta = 0.0f;
        bool m_FirstMouse = true;

        bool m_Dirty = false;
    };

}
