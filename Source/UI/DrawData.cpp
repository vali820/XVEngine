#include "DrawData.hpp"

UIDrawCmd::UIDrawCmd(const UIDrawCmdRoundedBox& _roundedBox) {
    kind       = UIDrawCmdKind::RoundedBox;
    roundedBox = _roundedBox;
}

UIDrawData::UIDrawData() { clear(); }

void UIDrawData::clear() {
    drawCommands.clear();
    color          = {1.0, 1.0, 1.0, 1.0};
    secondaryColor = {0.0, 0.0, 0.0, 1.0};
}

void UIDrawData::setColor(const Vec4& _color) { color = _color; }

void UIDrawData::setSecondaryColor(const Vec4& _secondaryColor) { secondaryColor = _secondaryColor; }

void UIDrawData::addRect(const Rect& rect) { addRect(rect, 0.0); }

void UIDrawData::addRect(const Rect& rect, float borderWidth) {
    addRect(rect, {borderWidth, borderWidth, borderWidth, borderWidth});
}

void UIDrawData::addRect(const Rect& rect, const Vec4& borderWidths) {
    addRoundedRect(rect, {0.0, 0.0, 0.0, 0.0}, borderWidths);
}

void UIDrawData::addRoundedRect(const Rect& rect, float radius) { addRoundedRect(rect, radius, 0.0); }

void UIDrawData::addRoundedRect(const Rect& rect, float radius, float borderWidth) {
    addRoundedRect(
        rect, {radius, radius, radius, radius}, {borderWidth, borderWidth, borderWidth, borderWidth});
}

void UIDrawData::addRoundedRect(const Rect& rect, const Vec4& radii) {
    addRoundedRect(rect, radii, {0.0, 0.0, 0.0, 0.0});
}

void UIDrawData::addRoundedRect(const Rect& rect, const Vec4& radii, const Vec4& borderWidths) {
    drawCommands.push(UIDrawCmdRoundedBox{
        .rect         = rect,
        .radii        = radii,
        .borderWidths = borderWidths,
        .color        = color,
        .borderColor  = secondaryColor,
    });
}
