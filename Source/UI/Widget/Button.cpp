#include "Button.hpp"

Vec2 Button::minSize() { return {100, 30}; }

void Button::draw(UIDrawData &drawData, const Vec2 &size) {
    drawData.addRoundedRect({0, 0, size.x, size.y}, 4, 1);
}