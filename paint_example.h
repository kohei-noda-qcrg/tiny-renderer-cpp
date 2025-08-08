#pragma once
#include <array>
#include <print>
#include <random>

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
    for(auto i = 0u; i < model.nfaces(); i++) {
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
    for(auto i = 0u; i < model.nfaces(); i++) {
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

inline auto paint_perspective_clown_model(std::vector<double>& zbuffer, TGAImage& framebuffer, const Model& model, const int width, const int height) {
    // viewport
    auto           rng    = std::mt19937(1);
    constexpr auto eye    = Vec3d(-1, 0, 2);
    constexpr auto center = Vec3d(0, 0, 0);
    constexpr auto up     = Vec3d(0, 1, 0);

    gl::ModelView   = gl::lookat(eye, center, up);
    gl::Perspective = gl::perspective(norm(eye - center));
    gl::ViewPort    = gl::viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
    for(auto i = 0u; i < model.nfaces(); i++) {
        auto clip = std::array<Vec4d, 3>();
        for(auto d = 0u; d < clip.size(); d++) {
            auto v  = model.vert(i, d);
            clip[d] = gl::Perspective * gl::ModelView * Vec4d(v.x, v.y, v.z, 1.0);
        }
        const auto rnd   = rng();
        const auto color = TGAColor((rnd >> 24) & 0xFF, (rnd >> 16) & 0xFF, (rnd >> 8) & 0xFF, rnd & 0xFF);
        gl::triangle(clip, zbuffer, framebuffer, color);
    }
}

inline auto paint_perspective_with_diffusemap(std::vector<double>& zbuffer, TGAImage& framebuffer, const Model& model, const int width, const int height) {
    //  viewport
    constexpr auto eye    = Vec3d(1, 1, 3);
    constexpr auto center = Vec3d(0, 0, 0);
    constexpr auto up     = Vec3d(0, 1, 0);

    gl::ModelView   = gl::lookat(eye, center, up);
    gl::Perspective = gl::perspective(norm(eye - center));
    gl::ViewPort    = gl::viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    auto shader     = gl::Shader(model);
    for(auto i = 0u; i < model.nfaces(); i++) {
        auto screen_coords = std::array<Vec4d, 3>();
        for(auto j = 0u; j < screen_coords.size(); j++) {
            screen_coords[j] = shader.vertex(i, j);
        }
        gl::triangle(screen_coords, shader, zbuffer, framebuffer);
    }
}

inline auto paint_diffuse_texture_with_eye(const Vec3d eye, std::vector<double>& zbuffer, TGAImage& framebuffer, const Model& model, const int width, const int height) {
    //  viewport
    constexpr auto center = Vec3d(0, 0, 0);
    constexpr auto up     = Vec3d(0, 1, 0);

    gl::ModelView   = gl::lookat(eye, center, up);
    gl::Perspective = gl::perspective(norm(eye - center));
    gl::ViewPort    = gl::viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    auto shader     = gl::Shader(model);
    for(auto i = 0u; i < model.nfaces(); i++) {
        auto screen_coords = std::array<Vec4d, 3>();
        for(auto j = 0u; j < screen_coords.size(); j++) {
            screen_coords[j] = shader.vertex(i, j);
        }
        gl::triangle(screen_coords, shader, zbuffer, framebuffer);
    }
}
