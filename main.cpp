#include <limits>
#include <print>
#include <random>

#include "geometry.h"
#include "gl.h"
#include "model.h"
#include "paint_example.h"
#include "tgaimage.h"

namespace {
constexpr auto width  = 800;
constexpr auto height = 800;

struct Shader : gl::IShader {
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
        const auto tex_interpolation = bar * varying_uv;
        const auto diffuse           = model.diffuse();
        const auto uv                = Vec2d(tex_interpolation.x * diffuse.get_width(), tex_interpolation.y * diffuse.get_height());
        color                        = diffuse.get(uv.x, uv.y);
        return false;
    }
};

auto rng = std::mt19937(1);
} // namespace

auto main(int argc, char** argv) -> int {
    auto framebuffer = TGAImage(width, height, TGAImage::RGB);
    auto zbuffer     = TGAImage(width, height, TGAImage::GRAYSCALE);

    // paint_sample_triangle(framebuffer);
    // load model
    ///*
    if(argc != 2) {
        std::println(stderr, "Usage: {} path/to/model.obj", argv[0]);
        return 1;
    }

    auto model = Model(argv[1]);
    if(!model.load_diffusemap(argv[1])) {
        return 1;
    }

    // paint_clown_model(zbuffer, framebuffer, model, width, height);
    // paint_illumination_model(zbuffer, framebuffer, model, width, height);

    // viewport
    ///*
    {
        constexpr auto eye    = Vec3d(-1, 0, 2);
        constexpr auto center = Vec3d(0, 0, 0);
        constexpr auto up     = Vec3d(0, 1, 0);

        gl::ModelView   = gl::lookat(eye, center, up);
        gl::Perspective = gl::perspective(norm(eye - center));
        gl::ViewPort    = gl::viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
        auto zbuffer    = std::vector<double>(width * height, std::numeric_limits<double>::max());
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
    //*/

    // viewport
    /*
    {
        constexpr auto eye    = Vec3d(1, 1, 3);
        constexpr auto center = Vec3d(0, 0, 0);
        constexpr auto up     = Vec3d(0, 1, 0);

        gl::ModelView   = gl::lookat(eye, center, up);
        gl::Perspective = gl::perspective(norm(eye - center));
        gl::ViewPort    = gl::viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
        auto shader     = Shader(model);
        auto zbuffer    = std::vector<double>(width * height, std::numeric_limits<double>::max());
        for(auto i = 0u; i < model.nfaces(); i++) {
            auto screen_coords = std::array<Vec4d, 3>();
            for(auto j = 0u; j < screen_coords.size(); j++) {
                screen_coords[j] = shader.vertex(i, j);
            }
            gl::triangle(screen_coords, shader, zbuffer, framebuffer);
        }
    }
    */

    framebuffer.write_tga_file("output.tga");
    return 0;
}
