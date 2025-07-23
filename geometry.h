#pragma once
#include <cstddef>

struct vec3 {
    double x = 0, y = 0, z = 0;

    auto operator[](size_t i) -> double& {
        return *(&x + i);
    }
    auto operator[](size_t i) const -> const double& {
        return *(&x + i);
    }
};
