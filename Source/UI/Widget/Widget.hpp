#pragma once

#include "../DrawData.hpp"
#include "Core/Core.hpp"

class Widget {
   public:
    Widget() = default;

    virtual Vec2 minSize();
    virtual void draw(UIDrawData& drawData, const Vec2& size) {}

};
