#pragma once

#include <format>

#include "../Types.hpp"
#include "Basic.hpp"

template <typename T>
class Vector2 {
   public:
    T x, y;

    Vector2() : x(0), y(0) {}

    explicit Vector2(T _x) : x(_x), y(_x) {}
    Vector2(T _x, T _y) : x(_x), y(_y) {}

    T length() const { return math::sqrt(x * x + y * y); }

    T distance(const Vector2& v) const { return length() - v.length(); }

    T dot(const Vector2& v) const { return x * v.x + y * v.y; }

    Vector2 normalize() const { return *this / length(); }

    Vector2& operator+=(const Vector2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    Vector2& operator-=(const Vector2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    Vector2& operator*=(const Vector2& v) {
        x *= v.x;
        y *= v.y;
        return *this;
    }
    Vector2& operator/=(const Vector2& v) {
        x /= v.x;
        y /= v.y;
        return *this;
    }
    Vector2& operator+=(T s) {
        x += s, y += s;
        return *this;
    }
    Vector2& operator-=(T s) {
        x -= s, y -= s;
        return *this;
    }
    Vector2& operator*=(T s) {
        x *= s;
        y *= s;
        return *this;
    }
    Vector2& operator/=(T s) {
        x /= s;
        y /= s;
        return *this;
    }

    Vector2 operator+(const Vector2& v) const { return {x + v.x, y + v.y}; }
    Vector2 operator-(const Vector2& v) const { return {x - v.x, y - v.y}; }
    Vector2 operator*(const Vector2& v) const { return {x * v.x, y * v.y}; }
    Vector2 operator/(const Vector2& v) const { return {x / v.x, y / v.y}; }
    Vector2 operator+(T s) const { return {x + s, y + s}; }
    Vector2 operator-(T s) const { return {x - s, y - s}; }
    Vector2 operator*(T s) const { return {x * s, y * s}; }
    Vector2 operator/(T s) const { return {x / s, y / s}; }

    Vector2 operator-() const { return {-x, -y}; }

    bool operator==(const Vector2& rhs) const { return x == rhs.x and y == rhs.y; }

    T& operator[](u32 i) { return (&x)[i]; }
    const T& operator[](u32 i) const { return (&x)[i]; }
};

template <typename T>
T distance(const Vector2<T>& a, const Vector2<T>& b) {
    return a.distance(b);
}

template <typename T>
T dot(const Vector2<T>& a, const Vector2<T>& b) {
    return a.dot(b);
}

using Vec2  = Vector2<f32>;
using DVec2 = Vector2<f64>;
using IVec2 = Vector2<i32>;

template <typename T>
struct std::formatter<Vector2<T>> : std::formatter<T> {
    auto format(const Vector2<T>& v, auto& ctx) const {
        return std::format_to(ctx.out(), "Vec2({}, {})", v.x, v.y);
    }
};