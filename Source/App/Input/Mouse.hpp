#pragma once

#include "Core/Core.hpp"

enum class MouseButton {
    Left,
    Right,
    Middle,
    _4,
    _5,
    BUTTON_COUNT,
};

const char* mouseButtonToString(MouseButton mouseButton);

class Mouse {
   private:
    bool buttons[(u32)MouseButton::BUTTON_COUNT];
    i32 x, y;

   public:
    Mouse();

    [[nodiscard]] inline bool isPressed(MouseButton mouseButton) const { return buttons[(u32)mouseButton]; }
    [[nodiscard]] inline i32 getX() const { return x; }
    [[nodiscard]] inline i32 getY() const { return y; }

    void setButton(MouseButton mouseButton, bool value);
    void setPos(i32 x, i32 y);
};

template <>
struct std::formatter<MouseButton> : std::formatter<String> {
    auto format(const MouseButton& b, auto& ctx) const {
        return std::format_to(ctx.out(), "{}", mouseButtonToString(b));
    }
};
