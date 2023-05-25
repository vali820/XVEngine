#pragma once

#include "Core/Core.hpp"

struct Size {
    u32 width, height;
};

class Widget {
   private:
    Widget* parent;

   public:
    explicit Widget(Widget* parent);

   protected:
    virtual Size minSize();
    virtual void draw() {}
};
