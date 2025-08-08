#include <limits>
#include <print>

#include "model.h"
#include "paint_example.h"
#include "tgaimage.h"

namespace {
constexpr auto width  = 800;
constexpr auto height = 800;
} // namespace

auto main(int argc, char** argv) -> int {
    auto framebuffer = TGAImage(width, height, TGAImage::RGB);

    // paint_sample_triangle(framebuffer);
    // load model
    if(argc != 2) {
        std::println(stderr, "Usage: {} path/to/model.obj", argv[0]);
        return 1;
    }

    auto model = Model(argv[1]);
    if(!model.load_diffusemap(argv[1])) {
        return 1;
    }

    /*
    {
        auto zbuffer = TGAImage(width, height, TGAImage::GRAYSCALE);
        // paint_clown_model(zbuffer, framebuffer, model, width, height);
        // paint_illumination_model(zbuffer, framebuffer, model, width, height);
    }
    */

    auto zbuffer = std::vector<double>(width * height, std::numeric_limits<double>::max());
    // paint_perspective_clown_model(zbuffer, framebuffer, model, width, height);
    paint_perspective_with_diffusemap<gl::Shader>(zbuffer, framebuffer, model, width, height);
    framebuffer.write_tga_file("output.tga");
    return 0;
}
