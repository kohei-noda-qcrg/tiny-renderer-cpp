#include <algorithm>
#include <cmath>
#include <numbers>
#include <print>
#include <type_traits>

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

template <typename T>
    requires(std::is_arithmetic_v<T>)
auto line(const vec2<T> pos1, const vec2<T> pos2, TGAImage& image, const TGAColor& color) -> void {
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

auto rotate(const Vec3d v) -> Vec3d {
    constexpr auto angle = std::numbers::pi / 6;
    const auto     ry    = mat<3, 3>{{{std::cos(angle), 0, std::sin(angle)},
                                      {0, 1, 0},
                                      {-std::sin(angle), 0, std::cos(angle)}}};
    return ry * v;
}

auto perspective(const Vec3d v) -> Vec3d {
    constexpr auto c = double(3);
    return v / (1 - v.z / c);
}

template <class T>
    requires(std::is_arithmetic_v<T>)
auto project(const Vec3d v) -> vec3<T> {
    return vec3<T>{
        T((v.x + 1) * width / 2),
        T((v.y + 1) * height / 2),
        T((v.z + 1) * 255.0 / 2),
    };
}

auto signed_triangle_area(const vec2<int> a, const vec2<int> b, const vec2<int> c) -> double {
    return 0.5 * ((b.y - a.y) * (b.x + a.x) + (c.y - b.y) * (c.x + b.x) + (a.y - c.y) * (a.x + c.x));
}

auto triangle(const std::array<vec3<int>, 3> t, TGAImage& zbuffer, TGAImage& framebuffer, const TGAColor& color) -> void {
    const auto boundary_box_min_x = std::max(0, std::min(std::min(t[0].x, t[1].x), t[2].x));
    const auto boundary_box_min_y = std::max(0, std::min(std::min(t[0].y, t[1].y), t[2].y));
    const auto boundary_box_max_x = std::min(int(framebuffer.get_width() - 1), std::max(std::max(t[0].x, t[1].x), t[2].x));
    const auto boundary_box_max_y = std::min(int(framebuffer.get_height() - 1), std::max(std::max(t[0].y, t[1].y), t[2].y));
    const auto t0vec2             = vec2<int>{t[0].x, t[0].y};
    const auto t1vec2             = vec2<int>{t[1].x, t[1].y};
    const auto t2vec2             = vec2<int>{t[2].x, t[2].y};
    const auto total_area         = signed_triangle_area(t0vec2, t1vec2, t2vec2);
    if(total_area < 1) return; // back-face culling
#pragma omp parallel for
    for(auto x = boundary_box_min_x; x <= boundary_box_max_x; x++) {
        for(auto y = boundary_box_min_y; y <= boundary_box_max_y; y++) {
            const auto pos   = vec2<int>(x, y);
            const auto alpha = signed_triangle_area(pos, t1vec2, t2vec2) / total_area;
            const auto beta  = signed_triangle_area(pos, t2vec2, t0vec2) / total_area;
            const auto gamma = signed_triangle_area(pos, t0vec2, t1vec2) / total_area;
            if(alpha < 0 || beta < 0 || gamma < 0) continue; // outside of the triangle
            const auto z = static_cast<uint8_t>(alpha * t[0].z + beta * t[1].z + gamma * t[2].z);
            if(z < zbuffer.get(x, y).raw[0]) continue;
            zbuffer.set(x, y, TGAColor(z, 1));
            framebuffer.set(x, y, color);
        }
    }
}
} // namespace

auto main(int argc, char** argv) -> int {
    auto framebuffer = TGAImage(width, height, TGAImage::RGB);
    auto zbuffer     = TGAImage(width, height, TGAImage::GRAYSCALE);

    // triangle
    /*
    const auto triangle0 = std::array{Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
    const auto triangle1 = std::array{Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
    const auto triangle2 = std::array{Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
    triangle(triangle0, framebuffer, red);
    triangle(triangle1, framebuffer, white);
    triangle(triangle2, framebuffer, green);
    */

    // load model
    ///*
    if(argc != 2) {
        std::println(stderr, "Usage: {} path/to/model.obj", argv[0]);
        return 1;
    }

    const auto model = Model(argv[1]);

    // clown colors, random
    ///*
    for(auto i = 0; i < model.nfaces(); i++) {
        const auto posa  = project<int>(perspective(rotate(model.vert(i, 0))));
        const auto posb  = project<int>(perspective(rotate(model.vert(i, 1))));
        const auto posc  = project<int>(perspective(rotate(model.vert(i, 2))));
        const auto color = TGAColor(std::rand() % 255, std::rand() % 255, std::rand() % 255, std::rand() % 255);
        triangle(std::array{posa, posb, posc}, zbuffer, framebuffer, color);
    }
    //*/

    // illmination from light_dir
    /*
    const auto light_dir = Vec3d(0, 0, -1);
    for(auto i = 0; i < model.nfaces(); i++) {
        auto screen_coords = std::array<Vec2i, 3>();
        auto world_coords  = std::array<Vec3d, 3>();
        for(auto j = 0; j < 3; j++) {
            const auto v     = model.vert(i, j);
            screen_coords[j] = Vec2i((v.x + 1) * width / 2, (v.y + 1) * height / 2);
            world_coords[j]  = v;
        }
        auto n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        const auto intensity = n * light_dir;
        std::println("intensity: {}", intensity);
        if(intensity > 0) {
            const auto color = TGAColor(intensity * 255, intensity * 255, intensity * 255, 255);
            triangle(screen_coords, framebuffer, color);
        }
    }
    */

    framebuffer.flip_vertically();
    zbuffer.flip_vertically();

    framebuffer.write_tga_file("output.tga");
    zbuffer.write_tga_file("zbuffer.tga");
    return 0;
}
