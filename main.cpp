#include <cmath>
#include <print>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

namespace {
constexpr auto width  = 800;
constexpr auto height = 800;

const auto white  = TGAColor(255, 255, 255, 255);
const auto green  = TGAColor(0, 255, 0, 255);
const auto red    = TGAColor(255, 0, 0, 255);
const auto blue   = TGAColor(64, 128, 255, 255);
const auto yellow = TGAColor(255, 200, 0, 255);
struct Position2D {
    int x;
    int y;
};

auto line(const Position2D pos1, const Position2D pos2, TGAImage& image, const TGAColor& color) -> void {
    auto start = pos1;
    auto end   = pos2;
    auto steep = std::abs(start.x - end.x) < std::abs(start.y - end.y);
    if(steep) {
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
    }
    if(start.x > end.x) {
        std::swap(start, end);
    }
    auto y      = start.y;
    auto ierror = 0;
    for(auto x = start.x; x <= end.x; x++) {
        auto realx = steep ? y : x;
        auto realy = steep ? x : y;
        image.set(realx, realy, color);
        ierror += 2 * std::abs(end.y - start.y);
        if(ierror > end.x - start.x) {
            y += end.y > start.y ? 1 : -1;
            ierror -= 2 * (end.x - start.x);
        }
    }
}

auto project(vec3 v) -> Position2D {
    return Position2D{int((v.x + 1) * width / 2), int((v.y + 1) * height / 2)};
}
} // namespace

auto main(int argc, char** argv) -> int {
    if(argc != 2) {
        std::println(stderr, "Usage: {} path/to/model.obj", argv[0]);
        return 1;
    }

    auto model       = Model(argv[1]);
    auto framebuffer = TGAImage(width, height, TGAImage::RGB);

    for(auto i = 0; i < model.nfaces(); i++) {
        auto posa = project(model.vert(i, 0));
        auto posb = project(model.vert(i, 1));
        auto posc = project(model.vert(i, 2));
        line(posa, posb, framebuffer, red);
        line(posb, posc, framebuffer, red);
        line(posc, posa, framebuffer, red);
    }

    for(auto i = 0; i < model.nverts(); i++) {
        auto v = model.vert(i);
        auto p = project(v);
        framebuffer.set(p.x, p.y, white);
    }
    framebuffer.flip_vertically();

    framebuffer.write_tga_file("output.tga");
    return 0;
}
