#ifndef ONCGL_WINDOW_WINDOW_H
#define ONCGL_WINDOW_WINDOW_H

#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "window/window.h"
#include "misc/constants.h"

namespace oncgl {

class Window {

 public:
  Window(std::string window_title, int width, int height);

  void init(void (*KeyCallback)(GLFWwindow *, int, int, int, int),
            void (*OnScrollCallback)(GLFWwindow *, double, double),
            void (*OnError)(int, const char *));

  GLFWwindow *window() const;

  float width() const;

  float height() const;

  /**
   * Tell the window it should close
   */
  void Close();

  /**
   * Check if window is marked as "should be closed"
   * @returns value of glfwWindowShouldClose
   */
  int ShouldClose();

 private:
  GLFWwindow *window_;

  float width_;
  float height_;
  std::string window_title_;
};

} // namespace oncgl

#endif // ONCGL_WINDOW_WINDOW_H
