#ifndef CG_PROJECT_DEFERRED_RENDERER_H
#define CG_PROJECT_DEFERRED_RENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader_program/shader_program.h"
#include "model/model.h"
#include "misc/constants.h"
#include "light/lights.h"
#include "camera/camera.h"
#include "framebuffer/framebuffer.h"

namespace oncgl {

struct Character {

  GLuint texture_id;
  glm::ivec2 size;
  glm::ivec2 bearing;
  FT_Pos advance;
};

class Renderer {
 public:

 private:

 protected:
  Renderer(float window_width, float window_height) : window_width_(
      window_width), window_height_(window_height) { }
  // path prefix for shaders, objects etc.

  float window_width_;
  float window_height_;

  Program *LoadShaders(std::string vertex_shader, std::string fragment_shader);
};

class DeferredRenderer : Renderer {

 public:

  DeferredRenderer(float window_width, float window_height);

  /**
   * Initialize the renderer
   * This function will load and compile all required shaders and initialize the
   * Framebuffer to draw in
   *
   * @param window_width  width of the window
   * @param window_height height of the window
   */
  void Init(int window_width, int window_height);

  /**
   * Render the geometrypass with the given models from cameras point of view
   *
   * @param models  models to draw
   * @param camera  camera to draw from
   */
  void RenderGeometryPass(std::vector<Model> models, Camera camera);

  /**
   * Render the stencilpass
   *
   * @param point_light   model of the pointlight
   * @param camera        camera to draw from
   */
  void RenderStencilPass(PointLight point_light, Camera camera);

  /**
   * Render the pointlightpass
   *
   * @param point_light   model of the pointlight
   * @param camera        camera to draw from
   */
  void RenderPointLightPass(PointLight point_light, Camera camera);

  /**
   * Render the directionallightpass
   *
   * @param directional_light   model of the directionallight
   * @param camera              camera to draw from
   */
  void RenderDirectionalLightPass(DirectionalLight directional_light,
                                  Camera camera);

  /**
   * Render the final pass
   * This pass will copy everything to the backbuffer
   */
  void RenderFinalPass();

 private:
  Program *geometryShaderProgram_;
  Program *pointLightShaderProgram_;
  Program *directionalLightShaderProgram_;
  Program *stencilShaderProgram_;

  // FrameBuffer
  FrameBuffer *frameBufferObject_;

  Model *pointLightModel_;
  Model *directionalLightModel_;
};

class FontRenderer : Renderer {
 public:
  FontRenderer(std::string font_path, unsigned int font_size,
               float window_width, float window_height);

  /**
   * Initialize the textrenderer
   * this function creates a orthogonal projection with the given width and height and
   * initialize the VAOs and VBOs
   */
  void Init(float width, float height);

  /**
   * Render the given text, at a given position and color.
   * The scale is a percent of 48 pixels of size
   *
   * @param text    text to draw
   * @param x       x-position to draw
   * @param y       y-position to draw
   * @param scale   scale relative to 48 pixel
   * @param color   color of text
   */
  void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale,
                  glm::vec3 color);

 private:
  Program *fontShaderProgram_;

  FT_Library ft_;
  FT_Face face_;

  GLuint VAO_, VBO_;
  std::map<GLchar, Character> characters_;

  glm::mat4 projection_;

  void CreateProjectionMatrix(float width, float height);
};

} // namespace oncgl

#endif // CG_PROJECT_DEFERRED_RENDERER_H
