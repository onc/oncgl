#include <stdexcept>
#include <lib/glew/include/GL/glew.h>

#include "window/window.h"

namespace oncgl {

Window::Window(std::string window_title, int width, int height) :
    width_(width),
    height_(height),
    window_title_(window_title) {
}

void Window::init(void (*KeyCallback)(GLFWwindow *, int, int, int, int),
                  void (*OnScrollCallback)(GLFWwindow *, double, double),
                  void (*OnError)(int, const char *)) {

  // initialise GLFW
  glfwSetErrorCallback(OnError);
  if (!glfwInit()) {
    throw std::runtime_error("[Window.cpp] Failed to init GLFW");
  }

  // open a window with GLFW
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, 8);
  window_ = glfwCreateWindow(width_, height_, window_title_.c_str(), NULL, NULL);
  if (!window_) {
    throw std::runtime_error(
        "[Window.cpp] glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");
  }

  // GLFW settings
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPos(window_, 0, 0);
  glfwSetScrollCallback(window_, OnScrollCallback);
  glfwMakeContextCurrent(window_);
  glfwSetKeyCallback(window_, KeyCallback);

  // initialise GLEW
  //stops glew crashing on OSX :-/
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("glewInit failed");
  }

  // enable mutlisampling
  glEnable(GL_MULTISAMPLE);

  // print out some info about the graphics drivers
  std::cout << K_GREEN << "OpenGL version: " << K_RESET <<
      glGetString(GL_VERSION) << std::endl;
  std::cout << K_GREEN << "GLSL version: " << K_RESET <<
      glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << K_GREEN << "Vendor: " << K_RESET << glGetString(GL_VENDOR) <<
      std::endl;
  std::cout << K_GREEN << "Renderer: " << K_RESET << glGetString(GL_RENDERER) <<
      std::endl;

  // make sure OpenGL version 3.2 API is available
  if (!GLEW_VERSION_3_2) {
    throw std::runtime_error("[Window.cpp] OpenGL 3.2 API is not available.");
  }
}

GLFWwindow *Window::window() const {
  return window_;
}

float Window::width() const {
  return width_;
}

float Window::height() const {
  return height_;
}

void Window::Close() {
  glfwSetWindowShouldClose(window_, GL_TRUE);
}

int Window::ShouldClose() {
  return glfwWindowShouldClose(window_);
}

} // namespace oncgl
