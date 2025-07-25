#include <algorithm>
#include <fstream>
#include <numeric>
#include <print>
#include <ranges>
#include <sstream>

#include "geometry.h"
#include "model.h"

Model::Model(const std::string_view filename) {
    auto in = std::ifstream();
    in.open(filename.data(), std::ifstream::in);
    if(!in) return;
    auto line = std::string();
    while(!in.eof()) {
        std::getline(in, line);
        auto iss   = std::istringstream(line.data());
        auto trash = char();
        if(line.starts_with("v ")) {
            // vertex
            iss >> trash;
            auto v = Vec3d{};
            for(auto i : std::views::iota(0, 3)) {
                iss >> v[i];
            }
            verts.push_back(v);
        } else if(line.starts_with("f ")) {
            auto f = 0, t = 0, n = 0, cnt = 0;
            iss >> trash;
            while(iss >> f >> trash >> t >> trash >> n) {
                facet_vrt.push_back(f - 1);
                cnt++;
            }
            if(cnt != 3) {
                std::println(stderr, "error: obj file is supposed to be triangulated.");
                return;
            }
        }
    }
    std::println(stderr, "# v# {} f# {}", nverts(), nfaces());

    auto idx = [&] {auto ret = std::vector<int>(nfaces()); std::iota(ret.begin(), ret.end(), 0); return ret; }();

    std::sort(idx.begin(), idx.end(),
              [&](const int a, const int b) {
                  const auto az_min = std::min(vert(a, 0).z, std::min(vert(a, 1).z, vert(a, 2).z));
                  const auto bz_min = std::min(vert(b, 0).z, std::min(vert(b, 1).z, vert(b, 2).z));
                  return az_min < bz_min;
              });

    auto orig_facet_vrt = facet_vrt;
    for(auto i = 0; i < nfaces(); i++) {
        std::copy_n(orig_facet_vrt.begin() + idx[i] * 3, 3, facet_vrt.begin() + i * 3);
    }
}

auto Model::nverts() const -> int { return verts.size(); }
auto Model::nfaces() const -> int { return facet_vrt.size() / 3; }
auto Model::vert(const int i) const -> Vec3d { return verts[i]; }
auto Model::vert(const int iface, const int nthvert) const -> Vec3d {
    return verts[facet_vrt[iface * 3 + nthvert]];
}
