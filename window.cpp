#include <print>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "paint_example.h"
#include "tgaimage.h"

namespace {
constexpr auto width  = 800;
constexpr auto height = 800;
} // namespace

auto main() -> int {

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

    auto image = TGAImage();
    if(!image.read_tga_file("output.tga")) {
        std::println(stderr, "failed to read tga file");
        return 1;
    }
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

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image.get_width(), image.get_height(), 0, format, GL_UNSIGNED_BYTE, image.buffer());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glViewport(0, 0, width, height);
    glClearColor(1.0, 1.0, 1.0, 0.0);

    while(glfwWindowShouldClose(window) == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glfwTerminate();
}
