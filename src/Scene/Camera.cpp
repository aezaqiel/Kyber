#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/Input.hpp"

namespace Kyber::Scene {

    Camera::Camera(f32 fov, f32 aspect, f32 near, f32 far)
        : m_FOV(fov), m_AspectRatio(aspect), m_NearClip(near), m_FarClip(far)
    {
        UpdateCameraVectors();
        RecalculateProjectionMatrix();
        RecalculateViewMatrix();

        auto [x, y] = Core::Input::GetMousePosition();
        m_LastMousePos = glm::vec2(x, y);
    }

    bool Camera::OnUpdate(const Core::Timer& timer)
    {
        bool moved = false;
        f32 velocity = m_MoveSpeed * timer.GetDeltaTime();

        if (std::abs(m_ScrollDelta) > 0.001f) {
            m_Position += m_Forward * m_ScrollDelta * m_ZoomSpeed;
            m_ScrollDelta = 0.0f;
            moved = true;
        }

        if (Core::Input::IsMouseButtonDown(Core::MouseButton::Right)) {
            auto [x, y] = Core::Input::GetMousePosition();
            glm::vec2 mousePos(x, y);

            if (m_FirstMouse) {
                m_LastMousePos = mousePos;
                m_FirstMouse = false;
            }

            glm::vec2 delta = (mousePos - m_LastMousePos) * m_MouseSens;
            m_LastMousePos = mousePos;

            if (glm::length(delta) > 0.001f) {
                glm::quat yawRotation = glm::angleAxis(
                    glm::radians(-delta.x * m_MouseSens),
                    m_WorldUp
                );

                glm::quat pitchRotation = glm::angleAxis(
                    glm::radians(-delta.y * m_MouseSens),
                    m_Right
                );

                m_Orientation = glm::normalize(yawRotation * m_Orientation);

                glm::vec3 nextForward = glm::normalize((m_Orientation * pitchRotation) * s_BaseForward);
                f32 pitchAngleDot = glm::dot(nextForward, m_WorldUp);

                if (std::abs(pitchAngleDot) < 0.9999f) {
                    m_Orientation = glm::normalize(m_Orientation * pitchRotation);
                }

                moved = true;
            }

            if (Core::Input::IsKeyDown(Core::KeyCode::W)) {
                m_Position += m_Forward * velocity;
                moved = true;
            } else if (Core::Input::IsKeyDown(Core::KeyCode::S)) {
                m_Position -= m_Forward * velocity;
                moved = true;
            }

            if (Core::Input::IsKeyDown(Core::KeyCode::A)) {
                m_Position -= m_Right * velocity;
                moved = true;
            } else if (Core::Input::IsKeyDown(Core::KeyCode::D)) {
                m_Position += m_Right * velocity;
                moved = true;
            }

            if (Core::Input::IsKeyDown(Core::KeyCode::E)) {
                m_Position += m_WorldUp * velocity;
                moved = true;
            } else if (Core::Input::IsKeyDown(Core::KeyCode::Q)) {
                m_Position -= m_WorldUp * velocity;
                moved = true;
            }
        } else {
            m_FirstMouse = true;
        }

        bool updated = moved || m_Dirty;

        if (moved) {
            UpdateCameraVectors();
            RecalculateViewMatrix();
        }

        if (m_Dirty) m_Dirty = false;

        return updated;
    }

    void Camera::OnEvent(Core::EventDispatcher& dispatcher)
    {
        dispatcher.Dispatch<Core::WindowResizedEvent>([&](const Core::WindowResizedEvent& e) -> bool {
            SetViewportSize(static_cast<f32>(e.width), static_cast<f32>(e.height));
            return false;
        });

        dispatcher.Dispatch<Core::MouseScrolledEvent>([&](const Core::MouseScrolledEvent& e) -> bool {
            m_ScrollDelta += e.y;
            return false;
        });
    }

    void Camera::SetViewportSize(f32 width, f32 height)
    {
        if (width <= 0 || height <= 0) {
            return;
        }

        m_AspectRatio = width / height;
        RecalculateProjectionMatrix();

        m_Dirty = true;
    }

    void Camera::RecalculateViewMatrix()
    {
        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
    }

    void Camera::RecalculateProjectionMatrix()
    {
        m_ProjMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }

    void Camera::UpdateCameraVectors()
    {
        m_Forward = glm::normalize(m_Orientation * s_BaseForward);
        m_Right = glm::normalize(m_Orientation * s_BaseRight);
        m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
    }

}
