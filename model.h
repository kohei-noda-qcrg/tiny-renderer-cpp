#pragma once
#include <string_view>
#include <vector>

#include "geometry.h"

class Model {
    std::vector<vec3> verts     = {};
    std::vector<int>  facet_vrt = {};

  public:
    Model(const std::string_view filename);
    auto nverts() const -> int;
    auto nfaces() const -> int;
    auto vert(const int i) const -> vec3;
    auto vert(const int iface, const int nthvert) const -> vec3;
};
