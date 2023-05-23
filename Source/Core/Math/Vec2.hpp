#pragma once

#include "../Types.hpp"

template <typename T>
class Vec2 {
   public:
    T x, y;

    Vec2() : x(0), y(0) {}

    explicit Vec2(T _x) : x(_x), y(_x) {}
    Vec2(T _x, T _y) : x(_x), y(_y) {}

    Vec2& operator+=(Vec2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vec2& operator-=(Vec2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vec2& operator*=(T s) {
        x *= s;
        y *= s;
        return *this;
    }

    Vec2& operator/=(T s) {
        x /= s;
        y /= s;
        return *this;
    }

    Vec2 operator+(const Vec2& rhs) { return {x + rhs.x, y + rhs.y}; }

    Vec2 operator-(const Vec2& rhs) { return {x - rhs.x, y - rhs.y}; }

    Vec2 operator*(T s) { return {x * s, y * s}; }

    Vec2 operator/(T s) { return {x / s, y / s}; }

    bool operator==(const Vec2& rhs) { return x == rhs.x and y == rhs.y; }

    T& operator[](u32 i) { return &x[i]; }
    const T& operator[](u32 i) const { return &x[i]; }
};

using FVec2 = Vec2<f32>;
using DVec2 = Vec2<f64>;
using IVec2 = Vec2<i32>;

template <typename T>
struct std::formatter<Vec2<T>> : std::formatter<T> {
    auto format(const Vec2<T>& v, auto& ctx) const {
        return std::format_to(ctx.out(), "Vec2({}, {})", v.x, v.y);
    }
};