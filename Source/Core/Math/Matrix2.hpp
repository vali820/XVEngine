#pragma once

#include "Vector2.hpp"

template <typename T>
class Matrix2 {
   private:
    Vector2<T> x, y;

   public:
    Matrix2() : Matrix2(1) {}

    explicit Matrix2(T s) : x(s, 0), y(0, s) {}

    Matrix2(const Vector2<T>& _x, const Vector2<T>& _y) : x(_x), y(_y) {}

    Matrix2(T c0r0, T c0r1, T c1r0, T c1r1) : x(c0r0, c0r1), y(c1r0, c1r1) {}

    Matrix2& operator+=(Matrix2& m) {
        x += m.x, y += m.y;
        return *this;
    }
    Matrix2& operator-=(Matrix2& m) {
        x -= m.x, y -= m.y;
        return *this;
    }
    Matrix2& operator/=(Matrix2& m) {
        x /= m.x, y /= m.y;
        return *this;
    }
    Matrix2& operator+=(T s) {
        x += s, y += s;
        return *this;
    }
    Matrix2& operator-=(T s) {
        x -= s, y -= s;
        return *this;
    }
    Matrix2& operator*=(T s) {
        x *= s, y *= s;
        return *this;
    }
    Matrix2& operator/=(T s) {
        x /= s, y /= s;
        return *this;
    }

    Matrix2 operator+(const Matrix2& m) const { return {x + m.x, y + m.y}; }
    Matrix2 operator-(const Matrix2& m) const { return {x - m.x, y - m.y}; }
    Matrix2 operator/(const Matrix2& m) const { return {x / m.x, y / m.y}; }
    Matrix2 operator+(T s) const { return {x + s, y + s}; }
    Matrix2 operator-(T s) const { return {x - s, y - s}; }
    Matrix2 operator*(T s) const { return {x * s, y * s}; }
    Matrix2 operator/(T s) const { return {x / s, y / s}; }

    Matrix2 operator*(const Matrix2<T>& m) { return {x * m.x.x + y * m.x.y, x * m.y.x + y * m.y.y}; }

    Matrix2 operator-() const { return {-x, -y}; }

    bool operator==(const Matrix2& m) const { return x == m.x and y == m.y; }

    Vector2<T>& operator[](u32 i) { return (&x)[i]; }
    const Vector2<T>& operator[](u32 i) const { return (&x)[i]; }

    // When clang supports this uncomment
    // T& operator[](u32 column, u32 row) { return &x[column][row]; }
    // const T& operator[](u32 column, u32 row) const { return &x[column][row]; }

    friend Vector2<T> operator*(const Vector2<T>&, const Matrix2&);
    friend Vector2<T> operator*(const Matrix2&, const Vector2<T>&);
};

template <typename T>
Vector2<T> operator*(const Vector2<T>& v, const Matrix2<T>& m) {
    return {
        m.x.x * v.x + m.x.y * v.y,
        m.y.x * v.x + m.y.y * v.y,
    };
}

template <typename T>
Vector2<T> operator*(const Matrix2<T>& m, const Vector2<T>& v) {
    return {
        m.x.x * v.x + m.y.x * v.y,
        m.x.y * v.x + m.y.y * v.y,
    };
}

using Mat2 = Matrix2<f32>;
using DMat2 = Matrix2<f64>;

template <typename T>
struct std::formatter<Matrix2<T>> : std::formatter<T> {
    auto format(const Matrix2<T>& m, auto& ctx) const {
        return std::format_to(ctx.out(), "Mat2(({}, {}), ({}, {}))", m[0][0], m[0][1], m[1][0], m[1][1]);
    }
};
