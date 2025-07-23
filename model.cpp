#include <fstream>
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
            auto v = vec3{};
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
}

auto Model::nverts() const -> int { return verts.size(); }
auto Model::nfaces() const -> int { return facet_vrt.size() / 3; }
auto Model::vert(const int i) const -> vec3 { return verts[i]; }
auto Model::vert(const int iface, const int nthvert) const -> vec3 {
    return verts[facet_vrt[iface * 3 + nthvert]];
}
