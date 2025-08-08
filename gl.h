#pragma once

#include <vector>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

namespace gl {
using Matrix = mat<4, 4>;

extern Matrix ModelView;
extern Matrix ViewPort;
extern Matrix Perspective;

struct IShader {
    virtual Vec4d vertex(const int iface, const int nthvert) = 0;
    virtual bool  fragment(const Vec3d bar, TGAColor& color) = 0;
};

struct Shader : IShader {
    const Model& model;
    mat<3, 2>    varying_uv;

    Shader(const Model& m) : model(m) {}

    virtual Vec4d vertex(const int iface, const int nthvert) {
        const auto vert     = model.vert(iface, nthvert);
        varying_uv[nthvert] = model.uv(iface, nthvert);
        const auto gl_pos   = gl::ModelView * Vec4d(vert.x, vert.y, vert.z, 1.0);
        return gl::Perspective * gl_pos;
    }

    virtual bool fragment(Vec3d bar, TGAColor& color) {
        const auto  tex_interpolation = bar * varying_uv;
        const auto& diffuse           = model.diffuse();
        const auto  uv                = Vec2d(tex_interpolation.x * diffuse.get_width(), tex_interpolation.y * diffuse.get_height());
        color                         = diffuse.get(uv.x, uv.y);
        return false;
    }
};

auto lookat(const Vec3d eye, const Vec3d center, const Vec3d up) -> mat<4, 4>;
auto perspective(const double f) -> mat<4, 4>;
auto viewport(const int x, const int y, const int w, const int h) -> mat<4, 4>;

auto rotate(const Vec3d v) -> Vec3d;
auto perspective(const Vec3d v) -> Vec3d;
auto triangle(const std::array<vec4<double>, 3> t, std::vector<double>& zbuffer, TGAImage& image, const TGAColor& color) -> void;
auto triangle(const std::array<vec4<double>, 3> t, IShader& shader, std::vector<double>& zbuffer, TGAImage& image) -> void;
auto triangle(const std::array<vec3<int>, 3> t, TGAImage& zbuffer, TGAImage& framebuffer, const TGAColor& color) -> void;
auto triangle(const std::array<vec2<int>, 3> t, TGAImage& framebuffer, const TGAColor& color) -> void;

template <Numeric T>
auto signed_triangle_area(const vec2<T> a, const vec2<T> b, const vec2<T> c) -> double {
    return 0.5 * ((b.y - a.y) * (b.x + a.x) + (c.y - b.y) * (c.x + b.x) + (a.y - c.y) * (a.x + c.x));
}
template <Numeric T>
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
template <Numeric T>
auto project(const Vec3d v, const size_t width, const size_t height) -> vec3<T> {
    return vec3<T>{
        T((v.x + 1) * width / 2),
        T((v.y + 1) * height / 2),
        T((v.z + 1) * 255.0 / 2),
    };
}
} // namespace gl
