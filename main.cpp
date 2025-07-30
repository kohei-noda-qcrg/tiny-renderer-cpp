#include <print>

#include "geometry.h"
#include "gl.h"
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
} // namespace

auto main(int argc, char** argv) -> int {
    auto framebuffer = TGAImage(width, height, TGAImage::RGB);
    auto zbuffer     = TGAImage(width, height, TGAImage::GRAYSCALE);

    // triangle
    /*
    const auto triangle0 = std::array{Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
    const auto triangle1 = std::array{Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
    const auto triangle2 = std::array{Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
    gl::triangle(triangle0, framebuffer, red);
    gl::triangle(triangle1, framebuffer, white);
    gl::triangle(triangle2, framebuffer, green);
    */

    // load model
    ///*
    if(argc != 2) {
        std::println(stderr, "Usage: {} path/to/model.obj", argv[0]);
        return 1;
    }

    const auto model = Model(argv[1]);

    // clown colors, random
    /*
    for(auto i = 0; i < model.nfaces(); i++) {
        const auto posa  = gl::project<int>(gl::perspective(gl::rotate(model.vert(i, 0))), width, height);
        const auto posb  = gl::project<int>(gl::perspective(gl::rotate(model.vert(i, 1))), width, height);
        const auto posc  = gl::project<int>(gl::perspective(gl::rotate(model.vert(i, 2))), width, height);
        const auto color = TGAColor(std::rand() % 255, std::rand() % 255, std::rand() % 255, std::rand() % 255);
        gl::triangle(std::array{posa, posb, posc}, zbuffer, framebuffer, color);
    }
    */

    // illmination from light_dir
    /*
    const auto light_dir = Vec3d(0, 0, -1);
    for(auto i = 0; i < model.nfaces(); i++) {
        auto screen_coords = std::array<Vec3i, 3>();
        auto world_coords  = std::array<Vec3d, 3>();
        for(auto j = 0; j < 3; j++) {
            const auto v     = model.vert(i, j);
            screen_coords[j] = Vec3i((v.x + 1) * width / 2, (v.y + 1) * height / 2, 0);
            world_coords[j]  = v;
        }
        auto n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n = normalized(n);
        const auto intensity = n * light_dir;
        std::println("intensity: {}", intensity);
        if(intensity > 0) {
            const auto color = TGAColor(intensity * 255, intensity * 255, intensity * 255, 255);
            gl::triangle(screen_coords, zbuffer, framebuffer, color);
        }
    }
    */
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
            const auto color = TGAColor(std::rand() % 255, std::rand() % 255, std::rand() % 255, std::rand() % 255);
            gl::rasterize(clip, ViewPort, zbuffer, framebuffer, color);
        }
    }
    //*/

    framebuffer.flip_vertically();

    framebuffer.write_tga_file("output.tga");
    return 0;
}
