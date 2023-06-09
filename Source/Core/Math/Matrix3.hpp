#pragma once

#include "Matrix2.hpp"
#include "Vector3.hpp"

template <typename T>
class Matrix3 {
   private:
    Vector3<T> x, y, z;

   public:
    Matrix3() : Matrix3(1) {}

    explicit Matrix3(T s) : x(s, 0, 0), y(0, s, 0), z(0, 0, s) {}

    Matrix3(const Vector3<T>& _x, const Vector3<T>& _y, const Vector3<T>& _z) : x(_x), y(_y), z(_z) {}

    Matrix3(T c0r0, T c0r1, T c0r2, T c1r0, T c1r1, T c1r2, T c2r0, T c2r1, T c2r2)
        : x(c0r0, c0r1, c0r2), y(c1r0, c1r1, c1r2), z(c2r0, c2r1, c2r2) {}

    Matrix2<T> toMat2() const { return {x.toVec2(), y.toVec2()}; }

    Matrix3& operator+=(Matrix3& m) {
        x += m.x, y += m.y, z += m.z;
        return *this;
    }
    Matrix3& operator-=(Matrix3& m) {
        x -= m.x, y -= m.y, z -= m.z;
        return *this;
    }
    Matrix3& operator/=(Matrix3& m) {
        x /= m.x, y /= m.y, z /= m.z;
        return *this;
    }
    Matrix3& operator+=(T s) {
        x += s, y += s, z += s;
        return *this;
    }
    Matrix3& operator-=(T s) {
        x -= s, y -= s, z -= s;
        return *this;
    }
    Matrix3& operator*=(T s) {
        x *= s, y *= s, z *= s;
        return *this;
    }
    Matrix3& operator/=(T s) {
        x /= s, y /= s, z /= s;
        return *this;
    }

    Matrix3 operator+(const Matrix3& m) const { return {x + m.x, y + m.y, z + m.z}; }
    Matrix3 operator-(const Matrix3& m) const { return {x - m.x, y - m.y, z - m.z}; }
    Matrix3 operator/(const Matrix3& m) const { return {x / m.x, y / m.y, z / m.z}; }
    Matrix3 operator+(T s) const { return {x + s, y + s, z + s}; }
    Matrix3 operator-(T s) const { return {x - s, y - s, z - s}; }
    Matrix3 operator*(T s) const { return {x * s, y * s, z * s}; }
    Matrix3 operator/(T s) const { return {x / s, y / s, z / s}; }

    Matrix3 operator*(const Matrix3& m) {
        return {x * m.x.x + y * m.x.y + z * m.x.z,
                x * m.y.x + y * m.y.y + z * m.y.z,
                x * m.z.x + y * m.z.y + z * m.z.z};
    }

    Matrix3 operator-() const { return {-x, -y, -z}; }

    bool operator==(const Matrix3& m) const { return x == m.x and y == m.y and z == m.z; }

    Vector3<T>& operator[](u32 i) { return (&x)[i]; }
    const Vector3<T>& operator[](u32 i) const { return (&x)[i]; }

    // When clang supports this uncomment
    // T& operator[](u32 column, u32 row) { return &x[column][row]; }
    // const T& operator[](u32 column, u32 row) const { return &x[column][row]; }

    friend Vector3<T> operator*(const Vector3<T>&, const Matrix3&);
    friend Vector3<T> operator*(const Matrix3&, const Vector3<T>&);
};

template <typename T>
Vector3<T> operator*(const Vector3<T>& v, const Matrix3<T>& m) {
    return {
        m.x.x * v.x + m.x.y * v.y + m.x.z * v.z,
        m.y.x * v.x + m.y.y * v.y + m.y.z * v.z,
        m.z.x * v.x + m.z.y * v.y + m.z.z * v.z,
    };
}

template <typename T>
Vector3<T> operator*(const Matrix3<T>& m, const Vector3<T>& v) {
    return {
        m.x.x * v.x + m.y.x * v.y + m.z.x * v.z,
        m.x.y * v.x + m.y.y * v.y + m.z.y * v.z,
        m.x.z * v.x + m.y.z * v.y + m.z.z * v.z,
    };
}

using Mat3 = Matrix3<f32>;
using DMat3 = Matrix3<f64>;

// clang-format off
template <typename T>
struct std::formatter<Matrix3<T>> : std::formatter<T> {
    auto format(const Matrix3<T>& m, auto& ctx) const {
        return std::format_to(ctx.out(),
                              "Mat3(({}, {}, {}), ({}, {}, {}), ({}, {}, {}))",
                              m[0][0], m[0][1], m[0][2],
                              m[1][0], m[1][1], m[1][2],
                              m[2][0], m[2][1], m[2][2]);
    }
};
// clang-format on