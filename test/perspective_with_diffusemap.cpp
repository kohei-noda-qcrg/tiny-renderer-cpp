#include <filesystem>
#include <print>

#include "paint_example.h"
#include "tgaimage.h"
#include "util.h"

namespace {
constexpr auto width  = 800;
constexpr auto height = 800;
} // namespace

auto main(const int argc, const char* argv[]) -> int {

    if(argc != 2) {
        std::println(stderr, "Usage: {} path/to/model.obj", argv[0]);
        return 1;
    }
    const auto filepath = std::filesystem::path(argv[1]);
    auto       model    = Model(filepath.string());
    if(!model.load_diffusemap(filepath.string())) {
        return 1;
    }
    auto framebuffer = TGAImage(width, height, TGAImage::RGB);
    auto zbuffer     = std::vector<double>(width * height, std::numeric_limits<double>::max());
    paint_perspective_with_diffusemap<gl::Shader>(zbuffer, framebuffer, model, width, height);

    const auto output = GEN_TEST_OUTPUT_NAME(filepath, ".tga");
    framebuffer.write_tga_file(output);
    return 0;
}
