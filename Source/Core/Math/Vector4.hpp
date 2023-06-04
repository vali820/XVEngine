#pragma once

#include "Vector3.hpp"

template <typename T>
class Vector4 {
   public:
    T x, y, z, w;

    Vector4() : x(0), y(0), z(0), w(0) {}

    explicit Vector4(T _x) : x(_x), y(_x), z(_x), w(_x) {}

    Vector4(T _x, T _y) : x(_x), y(_y), z(0) {}
    explicit Vector4(const Vector2<T>& xy) : x(xy.x), y(xy.y), z(0), w(0) {}

    Vector4(T _x, T _y, T _z) : x(_x), y(_y), z(_z), w(0) {}
    Vector4(const Vector2<T>& v, T _z) : x(v.x), y(v.y), z(_z), w(0) {}
    Vector4(T _x, const Vector2<T>& v) : x(_x), y(v.x), z(v.y), w(0) {}
    explicit Vector4(const Vector3<T>& v) : x(v.x), y(v.y), z(v.z), w(0) {}

    Vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vector4(const Vector2<T>& v, T _z, T _w) : x(v.x), y(v.y), z(_z), w(_w) {}
    Vector4(T _x, const Vector2<T>& v, T _w) : x(_x), y(v.x), z(v.y), w(_w) {}
    Vector4(T _x, T _y, const Vector2<T>& v) : x(_x), y(_y), z(v.x), w(v.y) {}
    Vector4(const Vector2<T>& v1, const Vector2<T>& v2) : x(v1.x), y(v1.y), z(v2.x), w(v2.y) {}
    Vector4(const Vector3<T>& v, T _w) : x(v.x), y(v.y), z(v.z), w(_w) {}
    Vector4(T _x, const Vector3<T>& v) : x(_x), y(v.x), z(v.y), w(v.z) {}

    Vector2<T> toVec2() const { return {x, y}; }
    Vector3<T> toVec3() const { return {x, y, z}; }

    T length() const { return math::sqrt(x * x + y * y + z * z + w * w); }

    T distance(const Vector4& v) const { return length() - v.length(); }

    T dot(const Vector4& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }

    Vector4 normalize() const { return *this / length(); }

    Vector4& operator+=(const Vector4& v) {
        x += v.x, y += v.y, z += v.z, w += v.w;
        return *this;
    }
    Vector4& operator-=(const Vector4& v) {
        x -= v.x, y -= v.y, z -= v.z, w -= v.w;
        return *this;
    }
    Vector4& operator*=(const Vector4& v) {
        x *= v.x, y *= v.y, z *= v.z, w *= v.w;
        return *this;
    }
    Vector4& operator/=(const Vector4& v) {
        x /= v.x, y /= v.y, z /= v.z, w /= v.w;
        return *this;
    }
    Vector4& operator+=(T s) {
        x += s, y += s, z += s, w += s;
        return *this;
    }
    Vector4& operator-=(T s) {
        x -= s, y -= s, z -= s, w -= s;
        return *this;
    }
    Vector4& operator*=(T s) {
        x *= s, y *= s, z *= s, w *= s;
        return *this;
    }
    Vector4& operator/=(T s) {
        x /= s, y /= s, z /= s, w /= s;
        return *this;
    }

    Vector4 operator+(const Vector4& v) const { return {x + v.x, y + v.y, z + v.z, w + v.w}; }
    Vector4 operator-(const Vector4& v) const { return {x - v.x, y - v.y, z - v.z, w - v.w}; }
    Vector4 operator*(const Vector4& v) const { return {x * v.x, y * v.y, z * v.z, w * v.w}; }
    Vector4 operator/(const Vector4& v) const { return {x / v.x, y / v.y, z / v.z, w / v.w}; }
    Vector4 operator+(T s) const { return {x + s, y + s, z + s, w + s}; }
    Vector4 operator-(T s) const { return {x - s, y - s, z - s, w - s}; }
    Vector4 operator*(T s) const { return {x * s, y * s, z * s, w * s}; }
    Vector4 operator/(T s) const { return {x / s, y / s, z / s, w / s}; }

    Vector4 operator-() const { return {-x, -y, -z, -w}; }

    bool operator==(const Vector4& v) const { return x == v.x and y == v.y and z == v.z and w == v.w; }

    T& operator[](u32 i) { return (&x)[i]; }
    const T& operator[](u32 i) const { return (&x)[i]; }
};

template <typename T>
T distance(const Vector4<T>& a, const Vector4<T>& b) {
    return a.distance(b);
}

template <typename T>
T dot(const Vector4<T>& a, const Vector4<T>& b) {
    return a.dot(b);
}

using Vec4  = Vector4<f32>;
using DVec4 = Vector4<f64>;
using IVec4 = Vector4<i32>;

template <typename T>
struct std::formatter<Vector4<T>> : std::formatter<T> {
    auto format(const Vector4<T>& v, auto& ctx) const {
        return std::format_to(ctx.out(), "Vec4({}, {}, {}, {})", v.x, v.y, v.z, v.w);
    }
};