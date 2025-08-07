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
    const auto filepath    = std::filesystem::path(argv[1]);
    const auto model       = Model(filepath.string());
    auto       framebuffer = TGAImage(width, height, TGAImage::RGB);
    auto       zbuffer     = TGAImage(width, height, TGAImage::GRAYSCALE);
    paint_clown_model(zbuffer, framebuffer, model, width, height);
    /*
    const auto loc = std::source_location::current();
    std::println("{}", std::filesystem::path(loc.file_name()).stem().string());
    const auto output = filepath.stem().string() + ".tga";
    */
    const auto output = GEN_TEST_OUTPUT_NAME(filepath, ".tga");

    framebuffer.write_tga_file(output);
    return 0;
}
