#pragma once

#include <cmath>
#include <numbers>

namespace math {
using std::abs, std::div, std::fmod, std::remainder, std::fmax, std::fmin, std::nan;
using std::ceil, std::floor, std::trunc, std::round;
using std::exp, std::exp2, std::log, std::log2, std::log10, std::log1p;
using std::isfinite, std::isinf, std::isnan, std::isnormal;
using std::pow, std::sqrt, std::cbrt, std::hypot;
using std::sin, std::cos, std::tan, std::asin, std::acos, std::atan, std::atan2;
using std::sinh, std::cosh, std::tanh, std::asinh, std::acosh, std::atanh;
using std::numbers::pi, std::numbers::e;

using std::clamp;

template <typename T>
T deg(T x) {
    return x * static_cast<T>(pi / 180.0);
}

template <typename T>
T inverseSqrt(T x) {
    return static_cast<T>(1) / std::sqrt(x);
}
}  // namespace math
