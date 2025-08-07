#include "paint_example.h"
#include "tgaimage.h"

namespace {
constexpr auto width  = 200;
constexpr auto height = 200;
} // namespace

auto main() -> int {
    auto framebuffer = TGAImage(width, height, TGAImage::RGB);
    paint_sample_triangle(framebuffer);
    framebuffer.write_tga_file("sample_triangle.tga");
}
