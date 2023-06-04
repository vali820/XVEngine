#pragma once

#include "Core/Core.hpp"

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
    KEY_COUNT,
};

const char* keyToString(Key key);

class Keyboard {
   private:
    bool keys[(u32)Key::KEY_COUNT];
    Modifiers mods;

   public:
    Keyboard();

    [[nodiscard]] inline bool isPressed(Key key) const { return keys[(u32)key]; }
    [[nodiscard]] inline const Modifiers& getModifiers(Key key) const { return mods; }

    void setKey(Key key, bool value);
    void setModifiers(const Modifiers& modifiers);
};

template <>
struct std::formatter<Modifiers> : std::formatter<String> {
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
struct std::formatter<Key> : std::formatter<String> {
    auto format(const Key& k, auto& ctx) const { return std::format_to(ctx.out(), "{}", keyToString(k)); }
};