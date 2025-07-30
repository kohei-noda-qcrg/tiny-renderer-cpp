#include "gl.h"

namespace gl {
auto lookat(const Vec3d eye, const Vec3d center, const Vec3d up) -> mat<4, 4> {
    const auto n = normalized(eye - center);
    const auto l = normalized(cross(up, n));
    const auto m = normalized(cross(n, l));
    return mat<4, 4>{{{l.x, l.y, l.z, 0}, {m.x, m.y, m.z, 0}, {n.x, n.y, n.z, 0}, {0, 0, 0, 1}}} * mat<4, 4>{{{1, 0, 0, -center.x}, {0, 1, 0, -center.y}, {0, 0, 1, -center.z}, {0, 0, 0, 1}}};
}

auto perspective(const double f) -> mat<4, 4> {
    return {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, -1 / f, 1}}};
}

auto viewport(const int x, const int y, const int w, const int h) -> mat<4, 4> {
    return {{{w / 2.0, 0, 0, x + w / 2.0}, {0, h / 2.0, 0, y + w / 2.0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
}
auto rasterize(const std::array<Vec4d, 3>& clip, const mat<4, 4>& viewport, std::vector<double>& zbuffer, TGAImage& framebuffer, const TGAColor& color) -> void {
    const auto ndc    = std::array<Vec4d, 3>{clip[0] / clip[0].w, clip[1] / clip[1].w, clip[2] / clip[2].w};
    const auto screen = std::array<Vec2d, 3>{(viewport * ndc[0]).xy(), (viewport * ndc[1]).xy(), (viewport * ndc[2]).xy()};

    const auto ABC = mat<3, 3>{{{screen[0].x, screen[0].y, 1.0}, {screen[1].x, screen[1].y, 1.0}, {screen[2].x, screen[2].y, 1.0}}};
    if(ABC.det() < 1) return;

    const auto [boundary_box_min_x, boundary_box_max_x] = std::minmax({screen[0].x, screen[1].x, screen[2].x});
    const auto [boundary_box_min_y, boundary_box_max_y] = std::minmax({screen[0].y, screen[1].y, screen[2].y});
#pragma omp parallel for
    for(auto x = std::max<int>(boundary_box_min_x, 0); x <= std::min<int>(boundary_box_max_x, framebuffer.get_width() - 1); x++) {
        for(auto y = std::max<int>(boundary_box_min_y, 0); y <= std::min<int>(boundary_box_max_y, framebuffer.get_height() - 1); y++) {
            const auto bc = ABC.invert_transpose() * Vec3d(x, y, 1);
            if(bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            const auto z       = bc * Vec3d(ndc[0].z, ndc[1].z, ndc[2].z);
            const auto buf_idx = x + y * framebuffer.get_width();
            if(z <= zbuffer[buf_idx]) continue;
            zbuffer[buf_idx] = z;
            framebuffer.set(x, y, color);
        }
    }
}

auto rotate(const Vec3d v) -> Vec3d {
    constexpr auto angle = std::numbers::pi / 6;
    const auto     ry    = mat<3, 3>{{{std::cos(angle), 0, std::sin(angle)},
                                      {0, 1, 0},
                                      {-std::sin(angle), 0, std::cos(angle)}}};
    return ry * v;
}

auto perspective(const Vec3d v) -> Vec3d {
    constexpr auto c = double(3);
    return v / (1 - v.z / c);
}

auto signed_triangle_area(const vec2<int> a, const vec2<int> b, const vec2<int> c) -> double {
    return 0.5 * ((b.y - a.y) * (b.x + a.x) + (c.y - b.y) * (c.x + b.x) + (a.y - c.y) * (a.x + c.x));
}

auto triangle(const std::array<vec3<int>, 3> t, TGAImage& zbuffer, TGAImage& framebuffer, const TGAColor& color) -> void {
    const auto boundary_box_min_x = std::max(0, std::min(std::min(t[0].x, t[1].x), t[2].x));
    const auto boundary_box_min_y = std::max(0, std::min(std::min(t[0].y, t[1].y), t[2].y));
    const auto boundary_box_max_x = std::min(int(framebuffer.get_width() - 1), std::max(std::max(t[0].x, t[1].x), t[2].x));
    const auto boundary_box_max_y = std::min(int(framebuffer.get_height() - 1), std::max(std::max(t[0].y, t[1].y), t[2].y));
    const auto t0vec2             = vec2<int>{t[0].x, t[0].y};
    const auto t1vec2             = vec2<int>{t[1].x, t[1].y};
    const auto t2vec2             = vec2<int>{t[2].x, t[2].y};
    const auto total_area         = signed_triangle_area(t0vec2, t1vec2, t2vec2);
    if(total_area < 1) return; // back-face culling
#pragma omp parallel for
    for(auto x = boundary_box_min_x; x <= boundary_box_max_x; x++) {
        for(auto y = boundary_box_min_y; y <= boundary_box_max_y; y++) {
            const auto pos   = vec2<int>(x, y);
            const auto alpha = signed_triangle_area(pos, t1vec2, t2vec2) / total_area;
            const auto beta  = signed_triangle_area(pos, t2vec2, t0vec2) / total_area;
            const auto gamma = signed_triangle_area(pos, t0vec2, t1vec2) / total_area;
            if(alpha < 0 || beta < 0 || gamma < 0) continue; // outside of the triangle
            const auto z = static_cast<uint8_t>(alpha * t[0].z + beta * t[1].z + gamma * t[2].z);
            if(z < zbuffer.get(x, y).raw[0]) continue;
            zbuffer.set(x, y, TGAColor(z, 1));
            framebuffer.set(x, y, color);
        }
    }
}
} // namespace gl
