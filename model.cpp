#include <filesystem>
#include <fstream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

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
        } else if(line.starts_with("vn ")) {
            iss >> trash >> trash;
            auto n = Vec3d{};
            for(auto i : std::views::iota(0, 3)) {
                iss >> n[i];
            }
            norms.push_back(normalized(n));
        } else if(line.starts_with("vt ")) {
            iss >> trash >> trash;
            auto t = Vec2d{};
            for(auto i : std::views::iota(0, 2)) {
                iss >> t[i];
            }
            tex.push_back({t.x, 1 - t.y});
        } else if(line.starts_with("f ")) {
            auto f = 0, t = 0, n = 0, cnt = 0;
            iss >> trash;
            while(iss >> f >> trash >> t >> trash >> n) {
                facet_vrt.push_back(f - 1);
                facet_tex.push_back(t - 1);
                facet_nrm.push_back(n - 1);
                cnt++;
            }
            if(cnt != 3) {
                std::println(stderr, "error: obj file is supposed to be triangulated.");
                return;
            }
        }
    }
    std::println(stderr, "# v# {} f# {} vt# {} vn# {} name# {}", nverts(), nfaces(), tex.size(), norms.size(), filename.data());
    // Painter's algorithm (too slow)
    /*
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
    */
}
auto Model::load_texture(std::string_view obj_file, std::string_view suffix, TGAImage& img) -> bool {
    const auto last_dot = obj_file.find_last_of(".");
    if(last_dot == std::string::npos) {
        std::println(stderr, "invalid filename: {}", obj_file);
        return false;
    }
    const auto filepath = std::format("{}{}", obj_file.substr(0, last_dot), std::string(suffix));
    if(!std::filesystem::exists(filepath)) {
        std::println(stderr, "{} does not exists", filepath);
        return false;
    }
    if(!img.read_tga_file(filepath.data())) {
        std::println(stderr, "failed to load {}", filepath);
        return false;
    }
    return true;
};

auto Model::load_diffusemap(std::string_view obj_file) -> bool {
    return load_texture(obj_file, "_diffuse.tga", diffusemap);
}

auto Model::nverts() const -> size_t { return verts.size(); }
auto Model::nfaces() const -> size_t { return facet_vrt.size() / 3; }
auto Model::vert(const int i) const -> Vec3d { return verts[i]; }
auto Model::vert(const int iface, const int nthvert) const -> Vec3d {
    return verts[facet_vrt[iface * 3 + nthvert]];
}
auto Model::uv(const int iface, const int nthvert) const -> Vec2d {
    return tex[facet_tex[iface * 3 + nthvert]];
}
auto Model::normal(const int iface, const int nthvert) const -> Vec3d {
    return norms[facet_nrm[iface * 3 + nthvert]];
}
