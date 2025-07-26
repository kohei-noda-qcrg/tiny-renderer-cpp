#pragma once
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

template <typename T>
    requires(std::is_arithmetic_v<T>)
using vec2 = glm::vec<2, T>;

using Vec2i = vec2<int32_t>;
using Vec2l = vec2<int64_t>;
using Vec2f = vec2<float>;
using Vec2d = vec2<double>;

template <typename T>
    requires(std::is_arithmetic_v<T>)
using vec3 = glm::vec<3, T>;

using Vec3i = vec3<int32_t>;
using Vec3l = vec3<int64_t>;
using Vec3f = vec3<float>;
using Vec3d = vec3<double>;

template <int nrows, int ncols>
using Mat = glm::mat<ncols, nrows, double>;
