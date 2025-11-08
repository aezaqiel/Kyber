#pragma once

#include "Event.hpp"
#include "Core/KeyCodes.hpp"

namespace Core {

    struct WindowClosedEvent final : public BaseEvent
    {
        constexpr WindowClosedEvent() noexcept = default;
    };

    struct WindowResizedEvent final : public BaseEvent
    {
        u32 width;
        u32 height;

        constexpr WindowResizedEvent(u32 width, u32 height) noexcept
            : width(width), height(height) {}
    };

    struct WindowMovedEvent final : public BaseEvent
    {
        i32 x;
        i32 y;

        constexpr WindowMovedEvent(i32 x, i32 y) noexcept
            : x(x), y(y) {}
    };

    struct WindowMinimizeEvent final : public BaseEvent
    {
        bool minimized;

        constexpr WindowMinimizeEvent(bool minimized) noexcept
            : minimized(minimized) {}
    };

    struct WindowFocusEvent final : public BaseEvent
    {
        bool focused;

        constexpr WindowFocusEvent(bool focused) noexcept
            : focused(focused) {}
    };

    struct KeyEvent : public BaseEvent
    {
        KeyCode keycode;

    protected:
        constexpr KeyEvent(KeyCode keycode) noexcept
            : keycode(keycode) {}
    };

    struct KeyPressedEvent final : public KeyEvent
    {
        bool repeat;

        constexpr KeyPressedEvent(KeyCode keycode, bool repeat) noexcept
            : KeyEvent(keycode), repeat(repeat) {}
    };

    struct KeyReleasedEvent final : public KeyEvent
    {
        constexpr KeyReleasedEvent(KeyCode keycode) noexcept
            : KeyEvent(keycode) {}
    };

    struct KeyTypedEvent final : public BaseEvent
    {
        u32 codepoint;

        constexpr KeyTypedEvent(u32 code) noexcept
            : codepoint(code) {}
    };

    struct MouseButtonEvent : public BaseEvent
    {
        MouseButton button;

    protected:
        constexpr MouseButtonEvent(MouseButton button) noexcept
            : button(button) {}
    };

    struct MouseButtonPressedEvent final : public MouseButtonEvent
    {
        constexpr MouseButtonPressedEvent(MouseButton button) noexcept
            : MouseButtonEvent(button) {}
    };

    struct MouseButtonReleasedEvent final : public MouseButtonEvent
    {
        constexpr MouseButtonReleasedEvent(MouseButton button) noexcept
            : MouseButtonEvent(button) {}
    };

    struct MouseEvent : public BaseEvent
    {
        f32 x;
        f32 y;

    protected:
        constexpr MouseEvent(f32 x, f32 y) noexcept
            : x(x), y(y) {}
    };

    struct MouseMovedEvent final : public MouseEvent
    {
        constexpr MouseMovedEvent(f32 x, f32 y) noexcept
            : MouseEvent(x, y) {}
    };

    struct MouseScrolledEvent final : public MouseEvent
    {
        constexpr MouseScrolledEvent(f32 x, f32 y) noexcept
            : MouseEvent(x, y) {}
    };

    using CoreEvents = EventVariant<
        WindowClosedEvent,
        WindowResizedEvent,
        WindowMovedEvent,
        WindowMinimizeEvent,
        WindowFocusEvent,
        KeyPressedEvent,
        KeyReleasedEvent,
        KeyTypedEvent,
        MouseButtonPressedEvent,
        MouseButtonReleasedEvent,
        MouseMovedEvent,
        MouseScrolledEvent
    >;

}
