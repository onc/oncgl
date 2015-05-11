#include <iostream>
#include <stdexcept>
#include <list>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "misc/constants.h"

#include "window/window.h"
#include "shader_program/shader_program.h"
#include "camera/camera.h"
#include "model/model.h"
#include "framebuffer/framebuffer.h"
#include "renderer/renderer.h"

// globals
oncgl::Window _window("Oncgl", 1600, 900);
double gScrollY = 0.0;
oncgl::Camera gCamera;

enum Direction {
  MOV_UP, MOV_DOWN, MOV_LEFT, MOV_RIGHT, NUM_DIRS
};
std::vector<bool> movement(Direction::NUM_DIRS);

enum RenderOptions {
  TOGGLE_DIR_LIGHT, TOGGLE_POINT_LIGHT, TOGGLE_DEBUG, NUM_OPS
};
std::vector<bool> renderToggles(RenderOptions::NUM_OPS);

oncgl::DeferredRenderer *deferredRenderer_;

std::vector<oncgl::Model> gModels;

std::vector<oncgl::PointLight> gPointLights;

oncgl::DirectionalLight gDirLight;

oncgl::FontRenderer *gFontRenderer;

// Callback for key events.
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {

  if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    movement[ Direction::MOV_UP ] = true;
  }
  if (key == GLFW_KEY_S &&
      action == GLFW_PRESS) { movement[ Direction::MOV_DOWN ] = true; }
  if (key == GLFW_KEY_A &&
      action == GLFW_PRESS) { movement[ Direction::MOV_LEFT ] = true; }
  if (key == GLFW_KEY_D &&
      action == GLFW_PRESS) { movement[ Direction::MOV_RIGHT ] = true; }

  if (key == GLFW_KEY_W &&
      action == GLFW_RELEASE) { movement[ Direction::MOV_UP ] = false; }
  if (key == GLFW_KEY_S &&
      action == GLFW_RELEASE) { movement[ Direction::MOV_DOWN ] = false; }
  if (key == GLFW_KEY_A &&
      action == GLFW_RELEASE) { movement[ Direction::MOV_LEFT ] = false; }
  if (key == GLFW_KEY_D &&
      action == GLFW_RELEASE) { movement[ Direction::MOV_RIGHT ] = false; }

  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    renderToggles[ RenderOptions::TOGGLE_POINT_LIGHT ]
        = !renderToggles[ RenderOptions::TOGGLE_POINT_LIGHT ];
  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    renderToggles[ RenderOptions::TOGGLE_DIR_LIGHT ]
        = !renderToggles[ RenderOptions::TOGGLE_DIR_LIGHT ];
  }
  if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
    renderToggles[ RenderOptions::TOGGLE_DEBUG ]
        = !renderToggles[ RenderOptions::TOGGLE_DEBUG ];
  }

  // Quit the program with 'q'
  if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
    _window.Close();
  }
}

static std::vector<oncgl::Model> LoadModels() {

  std::cout << "Loading models..." << std::endl;

  std::vector<oncgl::Model> models;

  oncgl::Model sphere(RESOURCE_DIRS_PREFIX + "../objects/sphere.obj");
  models.push_back(sphere);

  oncgl::Model plane2(RESOURCE_DIRS_PREFIX + "../objects/wooden_floor.obj");
  models.push_back(plane2);

  oncgl::Model monkeys(RESOURCE_DIRS_PREFIX + "../objects/monkeys.obj");
  models.push_back(monkeys);

  std::cout << K_GREEN << "Finished loading " << models.size() << " model(s)" <<
  K_RESET << std::endl;
  return models;
}

void Update() {

  // rotate camera based on mouse movement
  const float mouseSensitivity = 0.03f;
  double mouseX, mouseY;
  glfwGetCursorPos(_window.window(), &mouseX, &mouseY);
  float upAngle = mouseSensitivity * (float) mouseY;
  float rightAngle = mouseSensitivity * (float) mouseX;
  gCamera.offset_orientation(upAngle, rightAngle);
  glfwSetCursorPos(_window.window(), 0,
                   0); // reset the mouse, so it doesn't so out of the window

  if (movement[ Direction::MOV_UP ]) {
    gCamera.OffsetPosition(gCamera.Forward() * (0.2f));
  }
  if (movement[ Direction::MOV_DOWN ]) {
    gCamera.OffsetPosition(gCamera.Forward() * (-0.2f));
  }
  if (movement[ Direction::MOV_LEFT ]) {
    gCamera.OffsetPosition(gCamera.Right() * (-0.2f));
  }
  if (movement[ Direction::MOV_RIGHT ]) {
    gCamera.OffsetPosition(gCamera.Right() * (0.2f));
  }

  // increase or decrease field of view based on mouse wheel
  const float zoomSensitivity = -0.7f;
  float fieldOfView = gCamera.field_of_view() +
                      zoomSensitivity * (float) gScrollY;
  if (fieldOfView < 5.0f) { fieldOfView = 5.0f; }
  if (fieldOfView > 130.0f) { fieldOfView = 130.0f; }
  gCamera.set_field_of_view(fieldOfView);
  gScrollY = 0.0;
}

// records how far the y axis has been scrolled
void OnScroll(GLFWwindow *window, double deltaX, double deltaY) {
  gScrollY += deltaY;
}

void Render(int fps) {

  deferredRenderer_->Init(_window.width(), _window.height());

  deferredRenderer_->RenderGeometryPass(gModels, gCamera);

  if (renderToggles[ RenderOptions::TOGGLE_POINT_LIGHT ]) {
    glEnable(GL_STENCIL_TEST);
    for (int i = 0; i < gPointLights.size(); ++i) {
      deferredRenderer_->RenderStencilPass(gPointLights[ i ], gCamera);
      deferredRenderer_->RenderPointLightPass(gPointLights[ i ], gCamera);
    }
    glDisable(GL_STENCIL_TEST);
  }

  if (renderToggles[ RenderOptions::TOGGLE_DIR_LIGHT ]) {
    deferredRenderer_->RenderDirectionalLightPass(gDirLight, gCamera);
  }

  deferredRenderer_->RenderFinalPass();

  if (renderToggles[ RenderOptions::TOGGLE_DEBUG ]) {
    gFontRenderer->RenderText("fps: " + std::to_string(fps),
                              10, _window.height() - 30, 0.5f,
                              glm::vec3(1.0f, 1.0f, 1.0f));
    gFontRenderer->RenderText(
        "Press [1]: Toggle Point [2]: Toggle Dir [F3]: Toggle Debug [ESC]: Quit",
        10, 10, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
  }

  // Swap the buffers
  glfwSwapBuffers(_window.window());
}

void onError(int errorCode, const char *msg) {
  throw std::runtime_error(msg);
}

void AppMain() {

  // enable all render-options by default
  for (int i = 0; i < RenderOptions::NUM_OPS; i++) {
    renderToggles[ i ] = true;
  }

  _window.init(key_callback, OnScroll, onError);

  /************************************************
   *********** Load Models and draw ***************
   ************************************************/
  // setup camera
  gCamera.set_viewport_aspect_ratio(_window.width() / _window.height());
  // looking down to the scene
  gCamera.OffsetPosition(glm::vec3(0, 40.0f, 60.0f));
  gCamera.offset_orientation(30.0f, 0.0f);

  gModels = LoadModels();

  deferredRenderer_ = new oncgl::DeferredRenderer(_window.width(),
                                                  _window.height());

  for (float i = -10.0; i <= 10.0; i = i + 5.0) {
    for (float j = -10.0; j <= 10.0; j = j + 5.0) {

      oncgl::PointLight light;
      light.diffuse_intensity = 0.2f;
      light.ambient_intensity = 0.5f;
      light.color = COLOR_WHITE;
      light.position = glm::vec3(i * 7, 4.0f, j * 7);
      light.attenuation.constant = 0.8f;
      light.attenuation.linear = 0.8f;
      light.attenuation.exp = 2.0f;
      gPointLights.push_back(light);
    }
  }

  gDirLight.ambient_intensity = 0.8f;
  gDirLight.diffuse_intensity = 0.5f;
  gDirLight.color = COLOR_WHITE;
  gDirLight.direction = glm::vec3(-1.0f, -1.0f, 0.0f);

  gFontRenderer = new oncgl::FontRenderer(
      RESOURCE_DIRS_PREFIX + "../resources/fonts/OpenSans-Regular.ttf",
      48, _window.width(), _window.height());
  gFontRenderer->Init(_window.width(), _window.height());

  // glfwGetTime <- time in seconds but with micro-
  // or nanotime resolution as a double
  // fps counter
  double lastTime = glfwGetTime();
  unsigned int frames = 0;
  unsigned int fps = 0;

  // update
  unsigned int ticksPerSecond = 25;
  unsigned int skipTicks = 1 / ticksPerSecond;
  unsigned int maxFrameskip = 5;

  unsigned int loops;
  double nextTick = glfwGetTime();

  while (!_window.ShouldClose()) {
    // process pending events
    glfwPollEvents();

    // update the scene based on the time elapsed since last update
    double thisTime = glfwGetTime();
    frames++;

    // if more than 1sec past
    if (thisTime - lastTime >= 1.0) {
      fps = frames;
      frames = 0; lastTime = glfwGetTime();
    }

    loops = 0;
    // draw one frame
    while (glfwGetTime() > nextTick && loops < maxFrameskip) {
      Update();

      nextTick += skipTicks;
      loops++;
    }

    Render(fps);
  }

  // clean up and exit
  glfwTerminate();
}

int main(int argc, char *argv[]) {
  try {
    AppMain();
  } catch (const std::exception &e) {
    std::cerr << K_RED << "ERROR: " << e.what() << K_RESET << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
