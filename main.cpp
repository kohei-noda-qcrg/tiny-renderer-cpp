#include <print>
#include <random>

#include "model.h"
#include "paint_example.h"
#include "tgaimage.h"

namespace {
constexpr auto width  = 800;
constexpr auto height = 800;

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

    const auto model = Model(argv[1]);

    // paint_clown_model(zbuffer, framebuffer, model, width, height);
    // paint_illumination_model(zbuffer, framebuffer, model, width, height);

    // viewport
    ///*
    {
        constexpr auto eye    = Vec3d(-1, 0, 2);
        constexpr auto center = Vec3d(0, 0, 0);
        constexpr auto up     = Vec3d(0, 1, 0);

        const auto ModelView   = gl::lookat(eye, center, up);
        const auto Perspective = gl::perspective(norm(eye - center));
        const auto ViewPort    = gl::viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
        auto       zbuffer     = std::vector<double>(width * height, std::numeric_limits<double>::lowest());
        for(auto i = 0; i < model.nfaces(); i++) {
            auto clip = std::array<Vec4d, 3>();
            for(auto d = 0u; d < clip.size(); d++) {
                auto v  = model.vert(i, d);
                clip[d] = Perspective * ModelView * Vec4d(v.x, v.y, v.z, 1.0);
            }
            const auto rnd   = rng();
            const auto color = TGAColor((rnd >> 24) & 0xFF, (rnd >> 16) & 0xFF, (rnd >> 8) & 0xFF, rnd & 0xFF);
            gl::rasterize(clip, ViewPort, zbuffer, framebuffer, color);
        }
    }
    //*/

    framebuffer.flip_vertically();

    framebuffer.write_tga_file("output.tga");
    return 0;
}
