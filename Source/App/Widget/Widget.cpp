#include "Widget.hpp"

Widget::Widget(Widget *_parent) : parent(_parent) {}

Size Widget::minSize() { return {0, 0}; }