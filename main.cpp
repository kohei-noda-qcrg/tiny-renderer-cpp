#include <algorithm>
#include <cmath>
#include <concepts>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

namespace {
constexpr auto width  = 200;
constexpr auto height = 200;

const auto white  = TGAColor(255, 255, 255, 255);
const auto green  = TGAColor(0, 255, 0, 255);
const auto red    = TGAColor(255, 0, 0, 255);
const auto blue   = TGAColor(64, 128, 255, 255);
const auto yellow = TGAColor(255, 200, 0, 255);
struct Position2D {
    int x;
    int y;

    auto operator+(const Position2D& other) const -> Position2D {
        return Position2D{x + other.x, y + other.y};
    }

    auto operator-(const Position2D& other) const -> Position2D {
        return Position2D{x - other.x, y - other.y};
    }

    template <typename T>
        requires(std::floating_point<T> || std::integral<T>)
    auto operator*(const T scaler) const -> Position2D {
        return Position2D{int(x * scaler), int(y * scaler)};
    }
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

auto triangle(std::array<Position2D, 3> t, TGAImage& image, const TGAColor& color) -> void {
    // sort ascending y order
    if(t[0].y > t[1].y) std::swap(t[0], t[1]);
    if(t[0].y > t[2].y) std::swap(t[0], t[2]);
    if(t[1].y > t[2].y) std::swap(t[1], t[2]);

    const auto height = t[2].y - t[0].y;
    if(t[0].y != t[1].y) {
        // fill bottom half
        const auto segment_height = t[1].y - t[0].y;
        for(auto y = t[0].y; y <= t[1].y; y++) {
            const auto x1 = t[0].x + ((t[2].x - t[0].x) * (y - t[0].y)) / height;
            const auto x2 = t[0].x + ((t[1].x - t[0].x) * (y - t[0].y)) / segment_height;
            for(auto x = std::min(x1, x2); x < std::max(x1, x2); x++) {
                image.set(x, y, color);
            }
        }
    }
    if(t[1].y != t[2].y) {
        // fill upper half
        const auto segment_height = t[2].y - t[1].y;
        for(auto y = t[1].y; y <= t[2].y; y++) {
            const auto x1 = t[0].x + ((t[2].x - t[0].x) * (y - t[0].y)) / height;
            const auto x2 = t[1].x + ((t[2].x - t[1].x) * (y - t[1].y)) / segment_height;
            for(auto x = std::min(x1, x2); x < std::max(x1, x2); x++) {
                image.set(x, y, color);
            }
        }
    }
}
} // namespace

auto main(int argc, char** argv) -> int {
    auto framebuffer = TGAImage(width, height, TGAImage::RGB);

    // triangle
    const auto triangle0 = std::array<Position2D, 3>{Position2D(10, 70), Position2D(50, 160), Position2D(70, 80)};
    const auto triangle1 = std::array<Position2D, 3>{Position2D(180, 50), Position2D(150, 1), Position2D(70, 180)};
    const auto triangle2 = std::array<Position2D, 3>{Position2D(180, 150), Position2D(120, 160), Position2D(130, 180)};
    triangle(triangle0, framebuffer, red);
    triangle(triangle1, framebuffer, white);
    triangle(triangle2, framebuffer, green);

    // load model
    /*
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
    */
    framebuffer.flip_vertically();

    framebuffer.write_tga_file("output.tga");
    return 0;
}
