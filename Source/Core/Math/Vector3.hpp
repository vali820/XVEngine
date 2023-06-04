#pragma once

#include "Vector2.hpp"

template <typename T>
class Vector3 {
   public:
    T x, y, z;

    Vector3() : x(0), y(0), z(0) {}

    explicit Vector3(T _x) : x(_x), y(_x), z(_x) {}

    Vector3(T _x, T _y) : x(_x), y(_y), z(0) {}
    explicit Vector3(const Vector2<T>& v) : x(v.x), y(v.y), z(0) {}

    Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    Vector3(const Vector2<T>& v, T _z) : x(v.x), y(v.y), z(_z) {}
    Vector3(T _x, const Vector2<T>& v) : x(_x), y(v.x), z(v.y) {}

    Vector2<T> toVec2() const { return {x, y}; }

    T length() const { return math::sqrt(x * x + y * y + z * z); }

    T distance(const Vector3& v) const { return length() - v.length(); }

    T dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }

    Vector3 normalize() const { return *this / length(); }

    Vector3& operator+=(const Vector3& v) {
        x += v.x, y += v.y, z += v.z;
        return *this;
    }
    Vector3& operator-=(const Vector3& v) {
        x -= v.x, y -= v.y, z -= v.z;
        return *this;
    }
    Vector3& operator*=(const Vector3& v) {
        x *= v.x, y *= v.y, z *= v.z;
        return *this;
    }
    Vector3& operator/=(const Vector3& v) {
        x /= v.x, y /= v.y, z /= v.z;
        return *this;
    }
    Vector3& operator+=(T s) {
        x += s, y += s, z += s;
        return *this;
    }
    Vector3& operator-=(T s) {
        x -= s, y -= s, z -= s;
        return *this;
    }
    Vector3& operator*=(T s) {
        x *= s, y *= s, z *= s;
        return *this;
    }
    Vector3& operator/=(T s) {
        x /= s, y /= s, z /= s;
        return *this;
    }

    Vector3 operator+(const Vector3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vector3 operator-(const Vector3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vector3 operator*(const Vector3& v) const { return {x * v.x, y * v.y, z * v.z}; }
    Vector3 operator/(const Vector3& v) const { return {x / v.x, y / v.y, z / v.z}; }
    Vector3 operator+(T s) const { return {x + s, y + s, z + s}; }
    Vector3 operator-(T s) const { return {x - s, y - s, z - s}; }
    Vector3 operator*(T s) const { return {x * s, y * s, z * s}; }
    Vector3 operator/(T s) const { return {x / s, y / s, z / s}; }

    Vector3 operator-() const { return {-x, -y, -z}; }

    bool operator==(const Vector3& v) const { return x == v.x and y == v.y and z == v.z; }

    T& operator[](u32 i) { return (&x)[i]; }
    const T& operator[](u32 i) const { return (&x)[i]; }
};

template <typename T>
T distance(const Vector3<T>& a, const Vector3<T>& b) {
    return a.distance(b);
}

template <typename T>
T dot(const Vector3<T>& a, const Vector3<T>& b) {
    return a.dot(b);
}

using Vec3  = Vector3<f32>;
using DVec3 = Vector3<f64>;
using IVec3 = Vector3<i32>;

template <typename T>
struct std::formatter<Vector3<T>> : std::formatter<T> {
    auto format(const Vector3<T>& v, auto& ctx) const {
        return std::format_to(ctx.out(), "Vec3({}, {}, {})", v.x, v.y, v.z);
    }
};