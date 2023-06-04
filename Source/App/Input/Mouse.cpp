#include "Mouse.hpp"

Mouse::Mouse() : buttons(), x(0), y(0) {
    for (bool& button : buttons) button = false;
}

void Mouse::setButton(MouseButton mouseButton, bool value) { buttons[(u32)mouseButton] = value; }

void Mouse::setPos(i32 _x, i32 _y) { x = _x, y = _y; }

const char* mouseButtonToString(MouseButton mouseButton) {
    switch (mouseButton) {
        case MouseButton::Left: return "Left";
        case MouseButton::Right: return "Right";
        case MouseButton::Middle: return "Middle";
        case MouseButton::_4: return "Button4";
        case MouseButton::_5: return "Button5";
        case MouseButton::BUTTON_COUNT: return "";
    }
}