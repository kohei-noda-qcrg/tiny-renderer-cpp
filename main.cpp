#include <algorithm>
#include <cmath>
#include <concepts>
#include <print>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

namespace {
constexpr auto width  = 800;
constexpr auto height = 800;

const auto white  = TGAColor{255, 255, 255, 255};
const auto green  = TGAColor{0, 255, 0, 255};
const auto red    = TGAColor{255, 0, 0, 255};
const auto blue   = TGAColor{64, 128, 255, 255};
const auto yellow = TGAColor{255, 200, 0, 255};
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

    const auto steep = std::abs(start.x - end.x) < std::abs(start.y - end.y);
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
        const auto realx = steep ? y : x;
        const auto realy = steep ? x : y;
        image.set(realx, realy, color);
        ierror += 2 * std::abs(end.y - start.y);
        if(ierror > end.x - start.x) {
            y += end.y > start.y ? 1 : -1;
            ierror -= 2 * (end.x - start.x);
        }
    }
}

auto project(const vec3 v) -> Position2D {
    return Position2D{int((v.x + 1) * width / 2), int((v.y + 1) * height / 2)};
}

auto signed_triangle_area(const Position2D a, const Position2D b, const Position2D c) -> double {
    return 0.5 * ((b.y - a.y) * (b.x + a.x) + (c.y - b.y) * (c.x + b.x) + (a.y - c.y) * (a.x + c.x));
}

auto triangle(const std::array<Position2D, 3> t, TGAImage& image, const TGAColor& color) -> void {
    const auto boundary_box_min_x = std::min(std::min(t[0].x, t[1].x), t[2].x);
    const auto boundary_box_min_y = std::min(std::min(t[0].y, t[1].y), t[2].y);
    const auto boundary_box_max_x = std::max(std::max(t[0].x, t[1].x), t[2].x);
    const auto boundary_box_max_y = std::max(std::max(t[0].y, t[1].y), t[2].y);
    const auto total_area         = signed_triangle_area(t[0], t[1], t[2]);
    if(total_area < 1) return; // back-face culling
#pragma omp parallel for
    for(auto x = boundary_box_min_x; x <= boundary_box_max_x; x++) {
        for(auto y = boundary_box_min_y; y <= boundary_box_max_y; y++) {
            const auto pos   = Position2D(x, y);
            const auto alpha = signed_triangle_area(pos, t[1], t[2]) / total_area;
            const auto beta  = signed_triangle_area(pos, t[2], t[0]) / total_area;
            const auto gamma = signed_triangle_area(pos, t[0], t[1]) / total_area;
            if(alpha < 0 || beta < 0 || gamma < 0) continue; // outside of the triangle
            image.set(x, y, color);
        }
    }
}
} // namespace

auto main(int argc, char** argv) -> int {
    auto framebuffer = TGAImage(width, height, TGAImage::RGB);

    // triangle
    /*
    const auto triangle0 = std::array<Position2D, 3>{Position2D(10, 70), Position2D(50, 160), Position2D(70, 80)};
    const auto triangle1 = std::array<Position2D, 3>{Position2D(180, 50), Position2D(150, 1), Position2D(70, 180)};
    const auto triangle2 = std::array<Position2D, 3>{Position2D(180, 150), Position2D(120, 160), Position2D(130, 180)};
    triangle(triangle0, framebuffer, red);
    triangle(triangle1, framebuffer, white);
    triangle(triangle2, framebuffer, green);
    */

    // load model
    if(argc != 2) {
        std::println(stderr, "Usage: {} path/to/model.obj", argv[0]);
        return 1;
    }

    auto model = Model(argv[1]);

    for(auto i = 0; i < model.nfaces(); i++) {
        auto posa  = project(model.vert(i, 0));
        auto posb  = project(model.vert(i, 1));
        auto posc  = project(model.vert(i, 2));
        auto color = TGAColor(std::rand() % 255, std::rand() % 255, std::rand() % 255, std::rand() % 255);
        triangle(std::array{posa, posb, posc}, framebuffer, color);
    }

    framebuffer.flip_vertically();

    framebuffer.write_tga_file("output.tga");
    return 0;
}
