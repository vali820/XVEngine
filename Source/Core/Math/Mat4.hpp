#pragma once

#include "Mat3.hpp"
#include "Vec4.hpp"

template <typename T>
class Mat4 {
   private:
    Vec4<T> x, y, z, w;

   public:
    Mat4() : Mat4(1.0f) {}

    explicit Mat4(T s) : x(s, 0, 0, 0), y(0, s, 0, 0), z(0, 0, s, 0), w(0, 0, 0, s) {}

    Mat4(const Vec4<T>& _x, const Vec4<T>& _y, const Vec4<T>& _z, const Vec4<T>& _w)
        : x(_x), y(_y), z(_z), w(_w) {}

    Mat4(T c0r0, T c0r1, T c0r2, T c0r3, T c1r0, T c1r1, T c1r2, T c1r3, T c2r0, T c2r1, T c2r2, T c2r3,
         T c3r0, T c3r1, T c3r2, T c3r3)
        : x(c0r0, c0r1, c0r2, c0r3),
          y(c1r0, c1r1, c1r2, c1r3),
          z(c2r0, c2r1, c2r2, c2r3),
          w(c3r0, c3r1, c3r2, c3r3) {}

    Mat2<T> toMat2() const { return {x.toVec2(), y.toVec2()}; }
    Mat3<T> toMat3() const { return {x.toVec3(), y.toVec3(), z.toVec3()}; }

    Mat4& operator+=(Mat4& m) {
        x += m.x, y += m.y, z += m.z, w += m.w;
        return *this;
    }
    Mat4& operator-=(Mat4& m) {
        x -= m.x, y -= m.y, z -= m.z, w -= m.w;
        return *this;
    }
    Mat4& operator/=(Mat4& m) {
        x /= m.x, y /= m.y, z /= m.z, w /= m.w;
        return *this;
    }
    Mat4& operator+=(T s) {
        x += s, y += s, z += s, w += s;
        return *this;
    }
    Mat4& operator-=(T s) {
        x -= s, y -= s, z -= s, w -= s;
        return *this;
    }
    Mat4& operator*=(T s) {
        x *= s, y *= s, z *= s, w *= s;
        return *this;
    }
    Mat4& operator/=(T s) {
        x /= s, y /= s, z /= s, w /= s;
        return *this;
    }

    Mat4 operator+(const Mat4& m) const { return {x + m.x, y + m.y, z + m.z, w + m.w}; }
    Mat4 operator-(const Mat4& m) const { return {x - m.x, y - m.y, z - m.z, w - m.w}; }
    Mat4 operator/(const Mat4& m) const { return {x / m.x, y / m.y, z / m.z, w / m.w}; }
    Mat4 operator+(T s) const { return {x + s, y + s, z + s, w + s}; }
    Mat4 operator-(T s) const { return {x - s, y - s, z - s, w - s}; }
    Mat4 operator*(T s) const { return {x * s, y * s, z * s, w * s}; }
    Mat4 operator/(T s) const { return {x / s, y / s, z / s, w / s}; }

    Mat4 operator*(const Mat4& m) {
        return {x * m.x.x + y * m.x.y + z * m.x.z + w * m.x.w,
                x * m.y.x + y * m.y.y + z * m.y.z + w * m.y.w,
                x * m.z.x + y * m.z.y + z * m.z.z + w * m.z.w,
                x * m.w.x + y * m.w.y + z * m.w.z + w * m.w.w};
    }

    Mat4 operator-() const { return {-x, -y, -z, -w}; }

    bool operator==(const Mat4& m) const { return x == m.x and y == m.y and z == m.z and w == z.w; }

    Vec4<T>& operator[](u32 i) { return (&x)[i]; }
    const Vec4<T>& operator[](u32 i) const { return (&x)[i]; }

    // When clang supports this uncomment
    // T& operator[](u32 column, u32 row) { return &x[column][row]; }
    // const T& operator[](u32 column, u32 row) const { return &x[column][row]; }

    friend Vec4<T> operator*(const Vec4<T>&, const Mat4&);
    friend Vec4<T> operator*(const Mat4&, const Vec4<T>&);
};

template <typename T>
Vec4<T> operator*(const Vec4<T>& v, const Mat4<T>& m) {
    return {
        m.x.x * v.x + m.x.y * v.y + m.x.z * v.z + m.x.w * v.w,
        m.y.x * v.x + m.y.y * v.y + m.y.z * v.z + m.y.w * v.w,
        m.z.x * v.x + m.z.y * v.y + m.z.z * v.z + m.z.w * v.w,
        m.w.x * v.x + m.w.y * v.y + m.w.z * v.z + m.w.w * v.w,
    };
}

template <typename T>
Vec4<T> operator*(const Mat4<T>& m, const Vec4<T>& v) {
    return {
        m.x.x * v.x + m.y.x * v.y + m.z.x * v.z + m.w.x * v.w,
        m.x.y * v.x + m.y.y * v.y + m.z.y * v.z + m.w.y * v.w,
        m.x.z * v.x + m.y.z * v.y + m.z.z * v.z + m.w.z * v.w,
        m.x.w * v.x + m.y.w * v.y + m.z.w * v.z + m.w.w * v.w,
    };
}

using FMat4 = Mat4<f32>;
using DMat4 = Mat4<f64>;

// clang-format off
template <typename T>
struct std::formatter<Mat4<T>> : std::formatter<T> {
    auto format(const Mat4<T>& m, auto& ctx) const {
        return std::format_to(ctx.out(),
                              "Mat4(({}, {}, {}, {}), ({}, {}, {}, {}), ({}, {}, {}, {}), ({}, {}, {}, {}))",
                              m[0][0], m[0][1], m[0][2], m[0][3],
                              m[1][0], m[1][1], m[1][2], m[1][3],
                              m[2][0], m[2][1], m[2][2], m[2][3],
                              m[3][0], m[3][1], m[3][2], m[3][3]);
    }
};
// clang-format on