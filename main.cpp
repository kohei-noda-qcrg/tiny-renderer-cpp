#include <cmath>

#include "tgaimage.h"

namespace {
const auto white  = TGAColor(255, 255, 255, 255);
const auto green  = TGAColor(0, 255, 0, 255);
const auto red    = TGAColor(255, 0, 0, 255);
const auto blue   = TGAColor(64, 128, 255, 255);
const auto yellow = TGAColor(255, 200, 0, 255);
struct Position2D {
    int x;
    int y;
};
} // namespace

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
    for(auto x = start.x; x <= end.x; x++) {
        auto t     = (x - start.x) / static_cast<float>(end.x - start.x);
        auto y     = std::round(start.y + (end.y - start.y) * t);
        auto realx = steep ? y : x;
        auto realy = steep ? x : y;
        image.set(realx, realy, color);
    }
}

auto main() -> int {
    constexpr auto width       = 64;
    constexpr auto height      = 64;
    auto           framebuffer = TGAImage(width, height, TGAImage::RGB);

    const auto posa = Position2D{7, 3};
    const auto posb = Position2D{12, 37};
    const auto posc = Position2D{62, 53};
    line(posa, posb, framebuffer, blue);
    line(posc, posb, framebuffer, green);
    line(posc, posa, framebuffer, yellow);
    line(posa, posc, framebuffer, red);

    framebuffer.set(posa.x, posa.y, white);
    framebuffer.set(posb.x, posb.y, white);
    framebuffer.set(posc.x, posc.y, white);
    framebuffer.flip_vertically();

    framebuffer.write_tga_file("output.tga");
    return 0;
}
