#ifndef ONCGL_FRAMEBUFFER_FRAMEBUFFER_H
#define  ONCGL_FRAMEBUFFER_FRAMEBUFFER_H

#include <iostream>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>

#include "misc/constants.h"

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

namespace oncgl {

class FrameBuffer {
 public:
  // Enum for the buffertypes
  enum FRAMEBUFFER_TEXTURE_TYPE {
    FRAMEBUFFER_TEXTURE_TYPE_POSITION,
    FRAMEBUFFER_TEXTURE_TYPE_DIFFUSE,
    FRAMEBUFFER_TEXTURE_TYPE_NORMAL,
    FRAMEBUFFER_NUM_TEXTURES
  };

  FrameBuffer();

  ~FrameBuffer();

  /**
   * Initialize the framebuffer with given width and height
   *
   * @param window_width  (pixel) width of framebuffer
   * @param window_height (pixel) height of framebuffer
   * @returns true - if framebuffer is created successfully, otherwise false
   */
  bool Init(GLuint window_width, GLuint window_height);

  /**
   * Bind the framebuffer
   */
  void StartFrame();

  /**
   * Bind the framebuffer for the geometry-pass
   */
  void BindForGeometryPass();

  /**
   * Bind the framebuffer for the sencil-pass
   */
  void BindForStencilPass();

  /**
   * Bind the framebuffer for the light-passes
   */
  void BindForLightPass();

  /**
   * Bind the framebuffer for the final-rendering pass
   * The final pass will draw the framebuffer to the backbuffer
   */
  void BindForFinalPass();

 private:
  GLuint fbo_;
  GLuint textures_[FRAMEBUFFER_NUM_TEXTURES];
  GLuint depth_texture_;
  GLuint final_texture_;
};

} // namespace oncgl

#endif // ONCGL_FRAMEBUFFER_FRAMEBUFFER_H
