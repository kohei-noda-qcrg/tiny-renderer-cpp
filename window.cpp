#include <chrono>
#include <print>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "paint_example.h"
#include "tgaimage.h"

namespace {
constexpr auto width  = 800;
constexpr auto height = 800;

auto last_x      = 3.0;
auto last_y      = 0.0;
auto eye         = Vec3d(last_x, last_y, 3);
auto is_dragging = false;
void mouse_button_callback(GLFWwindow* window, int button, int action, int /* mods */) {
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        if(action == GLFW_PRESS) {
            is_dragging = true;
            glfwGetCursorPos(window, &last_x, &last_y);
        } else if(action == GLFW_RELEASE) {
            is_dragging = false;
        }
    }
}
void cursor_position_callback(GLFWwindow* /* window */, double xpos, double ypos) {
    if(is_dragging) {
        double dx = xpos - last_x;
        double dy = ypos - last_y;

        // Update camera 'eye' vector here based on dx and dy
        // For example, you could update a global 'eye' variable
        // The scaling factor (e.g., 0.1) controls the camera's movement speed
        eye.x += dx * 0.1;
        eye.y += dy * 0.1;

        last_x = xpos;
        last_y = ypos;
    }
}
struct FPS_Counter {
    FPS_Counter() : last_time(glfwGetTime()), frame_count(0), fps_(0) {};
    auto update() -> void {
        frame_count++;
        const auto current_time = glfwGetTime();
        if(current_time - last_time > period) {
            fps_        = frame_count / (current_time - last_time);
            frame_count = 0;
            last_time   = current_time;
        }
    }
    auto fps() -> double {
        return fps_;
    }

  private:
    const double period = 0.5;
    double       last_time;
    size_t       frame_count;
    double       fps_;
};
struct Timer {
    Timer() : start(std::chrono::steady_clock::now()), current(std::chrono::steady_clock::now()) {}
    auto now() -> void { current = std::chrono::steady_clock::now(); }
    auto duration() -> std::string { return std::format("{}", std::chrono::hh_mm_ss(std::chrono::duration_cast<std::chrono::milliseconds>(current - start))); }

  private:
    std::chrono::time_point<std::chrono::steady_clock> start, current;
};
} // namespace

auto main(const int argc, const char* argv[]) -> int {

    auto timer = Timer();
    if(argc != 2) {
        std::println(stderr, "Usage: {} path/to/model.obj", argv[0]);
        return 1;
    }

    auto image   = TGAImage(width, height, TGAImage::RGBA);
    auto zbuffer = std::vector<double>(width * height, std::numeric_limits<double>::max());
    auto model   = Model(argv[1]);
    if(!model.load_diffusemap(argv[1])) {
        return 1;
    }

    if(glfwInit() == GL_FALSE) {
        std::println(stderr, "failed to init glfw");
        return 1;
    }
    auto* window = glfwCreateWindow(width, height, "GLFW3", NULL /*GLFWmonitor *monitor*/, NULL /*GLFWwindow *share*/);
    if(!window) {
        std::println(stderr, "failed to create glfw window");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    auto format          = GL_RGBA;
    auto internal_format = GL_RGBA;
    switch(image.get_format()) {
    case TGAImage::RGBA:
        format          = GL_BGRA;
        internal_format = GL_RGBA;
        break;
    case TGAImage::RGB:
        format          = GL_BGR;
        internal_format = GL_RGB;
        break;
    case TGAImage::GRAYSCALE:
        format          = GL_RED;
        internal_format = GL_RED;
        break;
    }

    auto texture = GLuint();
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glViewport(0, 0, width, height);
    glClearColor(1.0, 1.0, 1.0, 0.0);

    auto frame_count = 0;
    auto fps_counter = FPS_Counter();
    while(glfwWindowShouldClose(window) == GL_FALSE) {
        std::fill(zbuffer.begin(), zbuffer.end(), std::numeric_limits<double>::max());
        image.fill(0);
        fps_counter.update();
        timer.now();
        std::print("\rFPS {:.1f}, {}, {} times rendered", fps_counter.fps(), timer.duration(), frame_count);
        std::fflush(stdout);
        frame_count++;
        paint_diffuse_texture_with_eye<gl::Shader>(eye, zbuffer, image, model, width, height);

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image.get_width(), image.get_height(), 0, format, GL_UNSIGNED_BYTE, image.buffer());
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glfwTerminate();
}
