#include "Input.hpp"

namespace Kyber {

    auto Input::IsKeyPressed(KeyCode key) -> bool
    {
        return s_KeyData.find(key) != s_KeyData.end() && s_KeyData[key].state == KeyState::Pressed;
    }

    auto Input::IsKeyHeld(KeyCode key) -> bool
    {
        return s_KeyData.find(key) != s_KeyData.end() && s_KeyData[key].state == KeyState::Held;
    }

    auto Input::IsKeyDown(KeyCode key) -> bool
    {
        return s_KeyData.find(key) != s_KeyData.end() && (s_KeyData[key].state == KeyState::Pressed || s_KeyData[key].state == KeyState::Held);
    }

    auto Input::IsKeyReleased(KeyCode key) -> bool
    {
        return s_KeyData.find(key) != s_KeyData.end() && s_KeyData[key].state == KeyState::Released;
    }

    auto Input::IsMouseButtonPressed(MouseButton button) -> bool
    {
        return s_MouseButtonData.find(button) != s_MouseButtonData.end() && s_MouseButtonData[button].state == KeyState::Pressed;
    }

    auto Input::IsMouseButtonHeld(MouseButton button) -> bool
    {
        return s_MouseButtonData.find(button) != s_MouseButtonData.end() && s_MouseButtonData[button].state == KeyState::Held;
    }

    auto Input::IsMouseButtonDown(MouseButton button) -> bool
    {
        return s_MouseButtonData.find(button) != s_MouseButtonData.end() && (s_MouseButtonData[button].state == KeyState::Pressed || s_MouseButtonData[button].state == KeyState::Held);
    }

    auto Input::IsMouseButtonReleased(MouseButton button) -> bool
    {
        return s_MouseButtonData.find(button) != s_MouseButtonData.end() && s_MouseButtonData[button].state == KeyState::Released;
    }

    auto Input::GetMouseX() -> f32
    {
        auto [x, y] = GetMousePosition();
        return static_cast<f32>(x);
    }

    auto Input::GetMouseY() -> f32
    {
        auto [x, y] = GetMousePosition();
        return static_cast<f32>(y);
    }

    auto Input::GetMousePosition() -> std::pair<f32, f32>
    {
        return s_MousePos;
    }

    auto Input::Update() -> void
    {
        for (auto& [key, data] : s_KeyData) {
            data.oldState = data.state;
            if (data.state == KeyState::Pressed) {
                data.state = KeyState::Held;
            } else if (data.state == KeyState::Released) {
                data.state = KeyState::None;
            }
        }

        for (auto& [button, data] : s_MouseButtonData) {
            data.oldState = data.state;
            if (data.state == KeyState::Pressed) {
                data.state = KeyState::Held;
            } else if (data.state == KeyState::Released) {
                data.state = KeyState::None;
            }
        }
    }

    auto Input::OnEvent(const EventDispatcher& dispatcher) -> void
    {
        dispatcher.Dispatch<KeyPressedEvent>([](const KeyPressedEvent& e) {
            if (!e.repeat) Input::UpdateKeyState(e.keycode, KeyState::Pressed);
            return false;
        });

        dispatcher.Dispatch<KeyReleasedEvent>([](const KeyReleasedEvent& e) {
            Input::UpdateKeyState(e.keycode, KeyState::Released);
            return false;
        });

        dispatcher.Dispatch<MouseButtonPressedEvent>([](const MouseButtonPressedEvent& e) {
            Input::UpdateButtonState(e.button, KeyState::Pressed);
            return false;
        });

        dispatcher.Dispatch<MouseButtonReleasedEvent>([](const MouseButtonReleasedEvent& e) {
            Input::UpdateButtonState(e.button, KeyState::Released);
            return false;
        });

        dispatcher.Dispatch<MouseMovedEvent>([](const MouseMovedEvent& e) {
            Input::UpdateMousePosition(e.x, e.y);
            return false;
        });
    }

    auto Input::UpdateKeyState(KeyCode key, KeyState state) -> void
    {
        auto& data = s_KeyData[key];
        data.oldState = data.state;
        data.state = state;
    }

    auto Input::UpdateButtonState(MouseButton button, KeyState state) -> void
    {
        auto& data = s_MouseButtonData[button];
        data.oldState = data.state;
        data.state = state;
    }

    auto Input::UpdateMousePosition(f32 x, f32 y) -> void
    {
        s_MousePos = std::make_pair(x, y);
    }

}
