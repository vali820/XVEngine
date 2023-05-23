#pragma once

#include <utility>

#include "Core/Core.hpp"

enum class MouseButton {
    Left,
    Right,
    Middle,
    _4,
    _5,
};

const char* mouseButtonToString(MouseButton mouseButton);

class Modifiers {
   private:
    u32 value;

   public:
    enum {
        MOD_SHIFT     = 1 << 0,
        MOD_CTRL      = 1 << 1,
        MOD_ALT       = 1 << 2,
        MOD_CAPS_LOCK = 1 << 3,
        MOD_NUM_LOCK  = 1 << 4,
    };

    inline explicit Modifiers(u32 _value) : value(_value) {}

    [[nodiscard]] inline bool shift() const { return value & MOD_SHIFT; }
    [[nodiscard]] inline bool ctrl() const { return value & MOD_CTRL; }
    [[nodiscard]] inline bool alt() const { return value & MOD_ALT; }
    [[nodiscard]] inline bool capsLock() const { return value & MOD_CAPS_LOCK; }
    [[nodiscard]] inline bool numLock() const { return value & MOD_NUM_LOCK; }

    inline void setShift(bool val) {
        if (val)
            value |= MOD_SHIFT;
        else
            value &= ~MOD_SHIFT;
    }
    inline void setCtrl(bool val) {
        if (val)
            value |= MOD_CTRL;
        else
            value &= ~MOD_CTRL;
    }
    inline void setAlt(bool val) {
        if (val)
            value |= MOD_ALT;
        else
            value &= ~MOD_ALT;
    }
    inline void setCapsLock(bool val) {
        if (val)
            value |= MOD_CAPS_LOCK;
        else
            value &= ~MOD_CAPS_LOCK;
    }
    inline void setNumLock(bool val) {
        if (val)
            value |= MOD_NUM_LOCK;
        else
            value &= ~MOD_NUM_LOCK;
    }

    [[nodiscard]] inline u32 getValue() const { return value; }
};

enum class Key : u32 {
    Unknown = 0,

    // Control
    Escape,
    CapsLock,
    LShift,
    RShift,
    LCtrl,
    RCtrl,
    LAlt,
    RAlt,
    Super,
    Menu,

    Tab,
    Enter,
    Backspace,
    Space,

    Grave,
    Minus,
    Equals,
    LBracket,
    RBracket,
    Backslash,
    Semicolon,
    Apostrophe,
    Comma,
    Period,
    Slash,

    Left,
    Right,
    Up,
    Down,
    Insert,
    Home,
    PageUp,
    PageDown,
    Delete,
    End,
    Print,
    ScrollLock,
    Pause,

    // Letters
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    // Numbers
    Num0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,

    // Function keys
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
};

const char* keyToString(Key key);

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
struct std::formatter<MouseButton> : std::formatter<std::string> {
    auto format(const MouseButton& b, auto& ctx) const {
        return std::format_to(ctx.out(), "{}", mouseButtonToString(b));
    }
};
template <>
struct std::formatter<Modifiers> : std::formatter<std::string> {
    auto format(const Modifiers& m, auto& ctx) const {
        std::string out;
        if (m.shift()) out += "Shift | ";
        if (m.ctrl()) out += "Ctrl | ";
        if (m.alt()) out += "Alt | ";
        if (m.capsLock()) out += "CapsLock | ";
        if (m.numLock()) out += "NumLock | ";
        if (out.ends_with(" | ")) out.erase(out.end() - 3, out.end());
        return std::format_to(ctx.out(), "{}", out);
    }
};
template <>
struct std::formatter<Key> : std::formatter<std::string> {
    auto format(const Key& k, auto& ctx) const { return std::format_to(ctx.out(), "{}", keyToString(k)); }
};
template <>
struct std::formatter<ResizeEvent> : std::formatter<std::string> {
    auto format(const ResizeEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "Resize(width: {}, height: {})", e.width, e.height);
    }
};
template <>
struct std::formatter<MouseMoveEvent> : std::formatter<std::string> {
    auto format(const MouseMoveEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "MouseMove(x: {}, y: {}, dx: {}, dy: {})", e.x, e.y, e.dx, e.dy);
    }
};
template <>
struct std::formatter<MouseButtonPressedEvent> : std::formatter<std::string> {
    auto format(const MouseButtonPressedEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "MouseButtonPressed({})", e.button);
    }
};
template <>
struct std::formatter<MouseButtonReleasedEvent> : std::formatter<std::string> {
    auto format(const MouseButtonReleasedEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "MouseButtonReleased({})", e.button);
    }
};
template <>
struct std::formatter<KeyPressedEvent> : std::formatter<std::string> {
    auto format(const KeyPressedEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "KeyPressed(key: {}, modifiers: {})", e.key, e.modifiers);
    }
};
template <>
struct std::formatter<KeyReleasedEvent> : std::formatter<std::string> {
    auto format(const KeyReleasedEvent& e, auto& ctx) const {
        return std::format_to(ctx.out(), "KeyReleased(key: {}, modifiers: {})", e.key, e.modifiers);
    }
};
