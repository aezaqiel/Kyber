#pragma once

#include "KeyCodes.hpp"
#include "Events.hpp"

namespace Kyber {

    struct KeyData
    {
        KeyState state { KeyState::None };
        KeyState oldState { KeyState::None };
    };

    struct ButtonData
    {
        KeyState state { KeyState::None };
        KeyState oldState { KeyState::None };
    };

    class Input
    {
        friend class Application;
    public:
        static auto IsKeyPressed(KeyCode key) -> bool;
        static auto IsKeyHeld(KeyCode key) -> bool;
        static auto IsKeyDown(KeyCode key) -> bool;
        static auto IsKeyReleased(KeyCode key) -> bool;

        static auto IsMouseButtonPressed(MouseButton button) -> bool;
        static auto IsMouseButtonHeld(MouseButton button) -> bool;
        static auto IsMouseButtonDown(MouseButton button) -> bool;
        static auto IsMouseButtonReleased(MouseButton button) -> bool;

        static auto GetMouseX() -> f32;
        static auto GetMouseY() -> f32;
        static auto GetMousePosition() -> std::pair<f32, f32>;

    protected:
        static auto Update() -> void;
        static auto OnEvent(const EventDispatcher& dispatcher) -> void;

    private:
        static auto UpdateKeyState(KeyCode key, KeyState state) -> void;
        static auto UpdateButtonState(MouseButton button, KeyState state) -> void;
        static auto UpdateMousePosition(f32 x, f32 y) -> void;

    private:
        inline static std::map<KeyCode, KeyData> s_KeyData;
        inline static std::map<MouseButton, ButtonData> s_MouseButtonData;
        inline static std::pair<f32, f32> s_MousePos { 0.0f, 0.0f };
    };

}
