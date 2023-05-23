#pragma once

#include "Vec3.hpp"

template <typename T>
class Vec4 {
   public:
    T x, y, z, w;

    Vec4() : x(0), y(0), z(0), w(0) {}

    explicit Vec4(T _x) : x(_x), y(_x), z(_x), w(_x) {}

    Vec4(T _x, T _y) : x(_x), y(_y), z(0) {}
    explicit Vec4(const Vec2<T>& xy) : x(xy.x), y(xy.y), z(0), w(0) {}

    Vec4(T _x, T _y, T _z) : x(_x), y(_y), z(_z), w(0) {}
    Vec4(const Vec2<T>& v, T _z) : x(v.x), y(v.y), z(_z), w(0) {}
    Vec4(T _x, const Vec2<T>& v) : x(_x), y(v.x), z(v.y), w(0) {}
    explicit Vec4(const Vec3<T>& v) : x(v.x), y(v.y), z(v.z), w(0) {}

    Vec4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vec4(const Vec2<T>& v, T _z, T _w) : x(v.x), y(v.y), z(_z), w(_w) {}
    Vec4(T _x, const Vec2<T>& v, T _w) : x(_x), y(v.x), z(v.y), w(_w) {}
    Vec4(T _x, T _y, const Vec2<T>& v) : x(_x), y(_y), z(v.x), w(v.y) {}
    Vec4(const Vec2<T>& v1, const Vec2<T>& v2) : x(v1.x), y(v1.y), z(v2.x), w(v2.y) {}
    Vec4(const Vec3<T>& v, T _w) : x(v.x), y(v.y), z(v.z), w(_w) {}
    Vec4(T _x, const Vec3<T>& v) : x(_x), y(v.x), z(v.y), w(v.z) {}

    Vec2<T> toVec2() const { return {x, y}; }
    Vec3<T> toVec3() const { return {x, y, z}; }

    Vec4& operator+=(Vec4& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    Vec4& operator-=(Vec4& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    Vec4& operator*=(T s) {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }

    Vec4& operator/=(T s) {
        x /= s;
        y /= s;
        z /= s;
        w /= s;
        return *this;
    }

    Vec4 operator+(const Vec4& v) { return {x + v.x, y + v.y, z + v.z, w + v.w}; }

    Vec4 operator-(const Vec4& v) { return {x - v.x, y - v.y, z - v.z, w - v.w}; }

    Vec4 operator*(T s) { return {x * s, y * s, z * s, w * s}; }

    Vec4 operator/(T s) { return {x / s, y / s, z / s, w / s}; }

    bool operator==(const Vec4& v) { return x == v.x and y == v.y and z == v.z and w == v.w; }

    T& operator[](u32 i) { return &x[i]; }
    const T& operator[](u32 i) const { return &x[i]; }
};

using FVec4 = Vec4<f32>;
using DVec4 = Vec4<f64>;
using IVec4 = Vec4<i32>;

template <typename T>
struct std::formatter<Vec4<T>> : std::formatter<T> {
    auto format(const Vec4<T>& v, auto& ctx) const {
        return std::format_to(ctx.out(), "Vec4({}, {}, {}, {})", v.x, v.y, v.z, v.w);
    }
};