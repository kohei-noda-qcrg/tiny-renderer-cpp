#pragma once
#include <cstddef>
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

struct vec3 {
    double x = 0, y = 0, z = 0;

    auto operator[](size_t i) -> double& {
        return *(&x + i);
    }
    auto operator[](size_t i) const -> const double& {
        return *(&x + i);
    }
};
