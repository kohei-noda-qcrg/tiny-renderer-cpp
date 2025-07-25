#pragma once
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>

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

    constexpr inline auto size() const -> size_t { return 3; }

    auto operator[](size_t i) -> T& {
        return *(&x + i);
    }
    auto operator[](size_t i) const -> const T& {
        return *(&x + i);
    }

    auto operator+(const vec3<T>& other) const -> vec3<T> {
        return vec3<T>{x + other.x, y + other.y, z + other.z};
    }

    auto operator-(const vec3<T>& other) const -> vec3<T> {
        return vec3<T>{x - other.x, y - other.y, z - other.z};
    }

    auto operator^(const vec3<T>& other) const -> vec3<T> {
        return vec3<T>{
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x};
    }

    template <class U>
        requires(std::is_arithmetic_v<U>)
    auto operator*(const U scale) const -> vec3<T> {
        return vec3<T>{x * T(scale), y * T(scale), z * T(scale)};
    }
    auto operator*(const vec3<T>& other) const -> T {
        return T(x * other.x + y * other.y + z * other.z);
    }

    auto norm() const -> T {
        return std::sqrt(x * x + y * y + z * z); // euclid norm
    }

    auto normalize() -> vec3<T> {
        *this = (*this) * (1 / norm());
        return *this;
    }
};

using Vec3i = vec3<int32_t>;
using Vec3l = vec3<int64_t>;
using Vec3f = vec3<float>;
using Vec3d = vec3<double>;

template <int nrows, int ncols>
class Mat {

    std::vector<std::vector<double>> data;

  public:
    Mat() : data(nrows, std::vector<double>(ncols, 0)) {};
    Mat(std::initializer_list<std::initializer_list<double>> init) {
        assert(init.size() == nrows);
        data.reserve(nrows);
        for(const auto& row : init) {
            assert(row.size() == ncols);
            data.emplace_back(row);
        }
    }
    inline auto rows() const -> int { return data.size(); }
    inline auto cols() const -> int { return data[0].size(); }

    std::vector<double>& operator[](int i) {
        assert(i >= 0 && i < rows());
        return data[i];
    }
    const std::vector<double>& operator[](int i) const {
        assert(i >= 0 && i < rows());
        return data[i];
    }
    template <class U>
        requires(std::is_arithmetic_v<U>)
    auto operator*(const vec3<U>& v) const -> vec3<U> {
        assert(rows() == v.size());
        auto res = vec3<U>(rows());

        for(auto i = 0; i < rows(); i++) {
            res[i] = 0;
            for(auto j = 0; j < cols(); j++) {
                res[i] += ((*this)[i][j]) * v[j];
            }
        }
        return res;
    }
};
