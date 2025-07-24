#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>

template <typename T>
    requires(std::is_arithmetic_v<T>)
struct vec2 {
    T x, y;

    auto operator+(const vec2<T>& other) const -> vec2<T> {
        return vec2<T>{x + other.x, y + other.y};
    }

    auto operator-(const vec2<T>& other) const -> vec2<T> {
        return vec2<T>{x - other.x, y - other.y};
    }

    template <typename U>
        requires(std::is_arithmetic_v<U>)
    auto operator*(const U scaler) const -> vec2<T> {
        return vec2<T>{T(x * scaler), T(y * scaler)};
    }
};

using Vec2i = vec2<int32_t>;
using Vec2l = vec2<int64_t>;
using Vec2f = vec2<float>;
using Vec2d = vec2<double>;

template <typename T>
    requires(std::is_arithmetic_v<T>)
struct vec3 {
    T x = 0, y = 0, z = 0;

    auto operator[](size_t i) -> T& {
        return *(&x + i);
    }
    auto operator[](size_t i) const -> const T& {
        return *(&x + i);
    }
};

using Vec3i = vec3<int32_t>;
using Vec3l = vec3<int64_t>;
using Vec3f = vec3<float>;
using Vec3d = vec3<double>;
