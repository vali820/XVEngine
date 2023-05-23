#pragma once

#include "Vec2.hpp"

template <typename T>
class Vec3 {
   public:
    T x, y, z;

    Vec3() : x(0), y(0), z(0) {}

    explicit Vec3(T _x) : x(_x), y(_x), z(_x) {}

    Vec3(T _x, T _y) : x(_x), y(_y), z(0) {}
    explicit Vec3(const Vec2<T>& v) : x(v.x), y(v.y), z(0) {}

    Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    Vec3(const Vec2<T>& v, T _z) : x(v.x), y(v.y), z(_z) {}
    Vec3(T _x, const Vec2<T>& v) : x(_x), y(v.x), z(v.y) {}

    Vec2<T> toVec2() const { return {x, y}; }

    Vec3& operator+=(Vec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vec3& operator-=(Vec3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vec3& operator*=(T s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vec3& operator/=(T s) {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    Vec3 operator+(const Vec3& v) { return {x + v.x, y + v.y, z + v.z}; }

    Vec3 operator-(const Vec3& v) { return {x - v.x, y - v.y, z - v.z}; }

    Vec3 operator*(T s) { return {x * s, y * s, z * s}; }

    Vec3 operator/(T s) { return {x / s, y / s, z / s}; }

    bool operator==(const Vec3& v) { return x == v.x and y == v.y and z == v.z; }

    T& operator[](u32 i) { return &x[i]; }
    const T& operator[](u32 i) const { return &x[i]; }
};

using FVec3 = Vec3<f32>;
using DVec3 = Vec3<f64>;
using IVec3 = Vec3<i32>;

template <typename T>
struct std::formatter<Vec3<T>> : std::formatter<T> {
    auto format(const Vec3<T>& v, auto& ctx) const {
        return std::format_to(ctx.out(), "Vec3({}, {}, {})", v.x, v.y, v.z);
    }
};