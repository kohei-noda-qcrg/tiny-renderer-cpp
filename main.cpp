#include "tgaimage.h"

namespace {
const auto white = TGAColor(255, 255, 255, 255);
const auto red   = TGAColor(255, 0, 0, 255);
struct Position2D {
    int x;
    int y;
};
} // namespace

auto line(const Position2D start, const Position2D end, const TGAColor& color, TGAImage& image) -> void {
    for(auto t = float(0); t < 1; t += 0.01) {
        auto x = int(start.x + (end.x - start.x) * t);
        auto y = int(start.y + (end.y - start.y) * t);
        image.set(x, y, color);
    }
}

auto main() -> int {
    auto image = TGAImage(100, 100, TGAImage::RGB);

    const auto start = Position2D{10, 80};
    const auto end   = Position2D{80, 60};
    line(start, end, red, image);
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}
