#pragma once

#include "Vec2.hpp"

template <typename T>
class Mat2 {
   private:
    Vec2<T> x, y;

   public:
    Mat2() : Mat2(1.0f) {}

    explicit Mat2(T s) : x(s, 0), y(0, s) {}

    Mat2(const Vec2<T>& _x, const Vec2<T>& _y) : x(_x), y(_y) {}

    Mat2(T c0r0, T c0r1, T c1r0, T c1r1) : x(c0r0, c0r1), y(c1r0, c1r1) {}

    Mat2& operator+=(Mat2& m) {
        x += m.x, y += m.y;
        return *this;
    }
    Mat2& operator-=(Mat2& m) {
        x -= m.x, y -= m.y;
        return *this;
    }
    Mat2& operator/=(Mat2& m) {
        x /= m.x, y /= m.y;
        return *this;
    }
    Mat2& operator+=(T s) {
        x += s, y += s;
        return *this;
    }
    Mat2& operator-=(T s) {
        x -= s, y -= s;
        return *this;
    }
    Mat2& operator*=(T s) {
        x *= s, y *= s;
        return *this;
    }
    Mat2& operator/=(T s) {
        x /= s, y /= s;
        return *this;
    }

    Mat2 operator+(const Mat2& m) const { return {x + m.x, y + m.y}; }
    Mat2 operator-(const Mat2& m) const { return {x - m.x, y - m.y}; }
    Mat2 operator/(const Mat2& m) const { return {x / m.x, y / m.y}; }
    Mat2 operator+(T s) const { return {x + s, y + s}; }
    Mat2 operator-(T s) const { return {x - s, y - s}; }
    Mat2 operator*(T s) const { return {x * s, y * s}; }
    Mat2 operator/(T s) const { return {x / s, y / s}; }

    Mat2 operator*(const Mat2<T>& m) { return {x * m.x.x + y * m.x.y, x * m.y.x + y * m.y.y}; }

    Mat2 operator-() const { return {-x, -y}; }

    bool operator==(const Mat2& m) const { return x == m.x and y == m.y; }

    Vec2<T>& operator[](u32 i) { return (&x)[i]; }
    const Vec2<T>& operator[](u32 i) const { return (&x)[i]; }

    // When clang supports this uncomment
    // T& operator[](u32 column, u32 row) { return &x[column][row]; }
    // const T& operator[](u32 column, u32 row) const { return &x[column][row]; }

    friend Vec2<T> operator*(const Vec2<T>&, const Mat2&);
    friend Vec2<T> operator*(const Mat2&, const Vec2<T>&);
};

template <typename T>
Vec2<T> operator*(const Vec2<T>& v, const Mat2<T>& m) {
    return {
        m.x.x * v.x + m.x.y * v.y,
        m.y.x * v.x + m.y.y * v.y,
    };
}

template <typename T>
Vec2<T> operator*(const Mat2<T>& m, const Vec2<T>& v) {
    return {
        m.x.x * v.x + m.y.x * v.y,
        m.x.y * v.x + m.y.y * v.y,
    };
}

using FMat2 = Mat2<f32>;
using DMat2 = Mat2<f64>;

template <typename T>
struct std::formatter<Mat2<T>> : std::formatter<T> {
    auto format(const Mat2<T>& m, auto& ctx) const {
        return std::format_to(ctx.out(), "Mat2(({}, {}), ({}, {}))", m[0][0], m[0][1], m[1][0], m[1][1]);
    }
};
