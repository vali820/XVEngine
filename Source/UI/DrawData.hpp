#pragma once

#include "Core/Core.hpp"
#include "Core/Math.hpp"

enum class UIDrawCmdKind {
    RoundedBox,
};

struct __attribute__((packed)) UIDrawCmdRoundedBox {
    Rect rect;
    Vec4 radii;
    Vec4 borderWidths;
    Vec4 color;
    Vec4 borderColor;
};

class UIDrawCmd {
   public:
    UIDrawCmd() = delete;
    UIDrawCmd(const UIDrawCmdRoundedBox& roundedBox);

    UIDrawCmdKind kind;

    union {
        UIDrawCmdRoundedBox roundedBox;
    };
};

class UIDrawData {
   private:
    Vec<UIDrawCmd> drawCommands;
    Vec4 color;
    Vec4 secondaryColor;
    Rect viewport;

   public:
    UIDrawData();

    void clear();

    void setViewport(const Rect& viewport);

    void setColor(const Vec4& color);
    void setSecondaryColor(const Vec4& secondaryColor);

    void addRect(const Rect& rect);
    void addRect(const Rect& rect, float borderWidth);
    void addRect(const Rect& rect, const Vec4& borderWidths);

    void addRoundedRect(const Rect& rect, float radius);
    void addRoundedRect(const Rect& rect, float radius, float borderWidth);
    void addRoundedRect(const Rect& rect, float radius, const Vec4& borderWidths);
    void addRoundedRect(const Rect& rect, const Vec4& radii);
    void addRoundedRect(const Rect& rect, const Vec4& radii, float borderWidth);
    void addRoundedRect(const Rect& rect, const Vec4& radii, const Vec4& borderWidths);

    [[nodiscard]] inline const Vec<UIDrawCmd>& getDrawCommands() const { return drawCommands; }
};
