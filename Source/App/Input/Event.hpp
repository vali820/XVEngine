#pragma once

#include "Core/Core.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

class ResizeEvent {
   public:
    u32 width, height;

    inline ResizeEvent(u32 _width, u32 _height) : width(_width), height(_height) {}
};

class MouseMoveEvent {
   public:
    i32 x, y;
    i32 dx, dy;

    inline MouseMoveEvent(i32 _x, i32 _y, i32 _dx, i32 _dy) : x(_x), y(_y), dx(_dx), dy(_dy) {}
};

class MouseButtonPressedEvent {
   public:
    MouseButton button;

    inline explicit MouseButtonPressedEvent(MouseButton _button) : button(_button) {}
};

class MouseButtonReleasedEvent {
   public:
    MouseButton button;

    inline explicit MouseButtonReleasedEvent(MouseButton _button) : button(_button) {}
};

class KeyPressedEvent {
   public:
    Key key;
    Modifiers modifiers;

    inline KeyPressedEvent(Key _key, Modifiers _modifiers) : key(_key), modifiers(_modifiers) {}
};

class KeyReleasedEvent {
   public:
    Key key;
    Modifiers modifiers;

    inline KeyReleasedEvent(Key _key, Modifiers _modifiers) : key(_key), modifiers(_modifiers) {}
};

template <>
struct std::formatter<ResizeEvent> : std::formatter<String> {
    auto format(const ResizeEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "Resize(width: {}, height: {})", e.width, e.height);
    }
};
template <>
struct std::formatter<MouseMoveEvent> : std::formatter<String> {
    auto format(const MouseMoveEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "MouseMove(x: {}, y: {}, dx: {}, dy: {})", e.x, e.y, e.dx, e.dy);
    }
};
template <>
struct std::formatter<MouseButtonPressedEvent> : std::formatter<String> {
    auto format(const MouseButtonPressedEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "MouseButtonPressed({})", e.button);
    }
};
template <>
struct std::formatter<MouseButtonReleasedEvent> : std::formatter<String> {
    auto format(const MouseButtonReleasedEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "MouseButtonReleased({})", e.button);
    }
};
template <>
struct std::formatter<KeyPressedEvent> : std::formatter<String> {
    auto format(const KeyPressedEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "KeyPressed(key: {}, modifiers: {})", e.key, e.modifiers);
    }
};
template <>
struct std::formatter<KeyReleasedEvent> : std::formatter<String> {
    auto format(const KeyReleasedEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "KeyReleased(key: {}, modifiers: {})", e.key, e.modifiers);
    }
};
