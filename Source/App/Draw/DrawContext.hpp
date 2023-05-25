#pragma once

#include "Core/Core.hpp"
#include "Core/Math.hpp"

struct Rect {
    IVec2 position;
    IVec2 size;
    u32 radius{};
    bool fill{};
    FVec4 color;
};

struct Line {
    IVec2 p1, p2;
    FVec4 color;
};

struct DrawData {
    Vec<Rect> rects;
    Vec<Line> lines;
};

class DrawContext {
   private:
    DrawData* drawData;
};
