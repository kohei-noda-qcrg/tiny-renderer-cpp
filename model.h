#pragma once
#include <string_view>
#include <vector>

#include "geometry.h"
#include "tgaimage.h"

class Model {
    std::vector<Vec3d> verts     = {};
    std::vector<Vec3d> norms     = {};
    std::vector<Vec3d> tex       = {};
    std::vector<int>   facet_vrt = {};
    std::vector<int>   facet_nrm = {};
    std::vector<int>   facet_tex = {};
    TGAImage           diffusemap;

  public:
    Model(const std::string_view filename);
    auto load_texture(const std::string_view obj_filename, const std::string_view suffix, TGAImage& img) -> bool;
    auto load_diffusemap(const std::string_view obj_filename) -> bool;
    auto nverts() const -> int;
    auto nfaces() const -> int;
    auto vert(const int i) const -> Vec3d;
    auto vert(const int iface, const int nthvert) const -> Vec3d;
    auto uv(const int iface, const int nthvert) const -> Vec3d;
    auto normal(const int iface, const int nthvert) const -> Vec3d;
};
