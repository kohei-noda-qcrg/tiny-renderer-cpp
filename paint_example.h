#pragma once
#include <array>
#include <print>

#include "color.h"
#include "geometry.h"
#include "gl.h"
#include "model.h"
#include "tgaimage.h"

inline auto paint_sample_triangle(TGAImage& framebuffer) -> void {
    // triangle
    const auto triangle0 = std::array{Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
    const auto triangle1 = std::array{Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
    const auto triangle2 = std::array{Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
    gl::triangle(triangle0, framebuffer, color::red);
    gl::triangle(triangle1, framebuffer, color::white);
    gl::triangle(triangle2, framebuffer, color::green);
}

inline auto paint_clown_model(TGAImage& zbuffer, TGAImage& framebuffer, const Model& model, const int width, const int height) -> void {
    // clown colors, random
    for(auto i = 0; i < model.nfaces(); i++) {
        const auto posa  = gl::project<int>(gl::perspective(gl::rotate(model.vert(i, 0))), width, height);
        const auto posb  = gl::project<int>(gl::perspective(gl::rotate(model.vert(i, 1))), width, height);
        const auto posc  = gl::project<int>(gl::perspective(gl::rotate(model.vert(i, 2))), width, height);
        const auto color = TGAColor(std::rand() % 255, std::rand() % 255, std::rand() % 255, std::rand() % 255);
        gl::triangle(std::array{posa, posb, posc}, zbuffer, framebuffer, color);
    }
}

inline auto paint_illumination_model(TGAImage& zbuffer, TGAImage& framebuffer, const Model& model, const int width, const int height) -> void {
    // illmination from light_dir
    const auto light_dir = Vec3d(0, 0, -1);
    for(auto i = 0; i < model.nfaces(); i++) {
        auto screen_coords = std::array<Vec3i, 3>();
        auto world_coords  = std::array<Vec3d, 3>();
        for(auto j = 0; j < 3; j++) {
            const auto v     = model.vert(i, j);
            screen_coords[j] = Vec3i((v.x + 1) * width / 2, (v.y + 1) * height / 2, 0);
            world_coords[j]  = v;
        }
        const auto n         = normalized(cross(world_coords[2] - world_coords[0], world_coords[1] - world_coords[0]));
        const auto intensity = n * light_dir;
        std::println("intensity: {}", intensity);
        if(intensity > 0) {
            const auto color = TGAColor(intensity * 255, intensity * 255, intensity * 255, 255);
            gl::triangle(screen_coords, zbuffer, framebuffer, color);
        }
    }
}
