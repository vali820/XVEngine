#pragma once

#include "Widget.hpp"

class Button : public Widget {
   protected:
    Vec2 minSize() override;
    void draw(UIDrawData &drawData, const Vec2 &size) override;

   public:
    Button() = default;
};
