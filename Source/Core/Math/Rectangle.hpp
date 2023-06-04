#pragma once

#include "../Core.hpp"
#include "Vector2.hpp"

template <typename T>
class Rectangle {
   public:
    T x, y, w, h;

    Rectangle() : x(0), y(0), w(0), h(0) {}
    Rectangle(T _x, T _y, T _w, T _h) : x(_x), y(_y), w(_w), h(_h) {}
    Rectangle(const Vector2<T>& position, T _w, T _h) : x(position.x), y(position.y), w(_w), h(_h) {}
    Rectangle(T _x, T _y, const Vector2<T>& size) : x(_x), y(_y), w(size.x), h(size.y) {}
    Rectangle(const Vector2<T>& position, const Vector2<T>& size)
        : x(position.x), y(position.y), w(size.x), h(size.y) {}

    Vector2<T> getPosition() const { return {x, y}; }
    Vector2<T> getSize() const { return {w, h}; }

    Vector2<T> getTopLeft() const { return {x, y}; }
    Vector2<T> getTopRight() const { return {x + w, y}; }
    Vector2<T> getBottomLeft() const { return {x, y + h}; }
    Vector2<T> getBottomRight() const { return {x + w, y + h}; }
    Vector2<T> getCenter() const { return {x + w / static_cast<T>(2), y + h / static_cast<T>(2)}; }

    Rectangle viewportTransform(const Rectangle& rect) {
        return {getPosition() + rect.getPosition(), math::min(w, rect.w), math::min(h, rect.h)};
    }
};

using Rect  = Rectangle<f32>;
using DRect = Rectangle<f64>;
using IRect = Rectangle<i32>;
