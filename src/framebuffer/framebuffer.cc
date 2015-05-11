#include "framebuffer.h"

namespace oncgl {

FrameBuffer::FrameBuffer() {

  fbo_ = 0;
  depth_texture_ = 0;
  ZERO_MEM(textures_);
}

FrameBuffer::~FrameBuffer() {

  if (fbo_ != 0) {
    glDeleteFramebuffers(1, &fbo_);
  }

  if (textures_[ 0 ] != 0) {
    glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(textures_), textures_);
  }

  if (depth_texture_ != 0) {
    glDeleteTextures(1, &depth_texture_);
  }
}

bool FrameBuffer::Init(GLuint window_width, GLuint window_height) {

  // Create the FBO
  glGenFramebuffers(1, &fbo_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);

  std::cout << "Create Framebuffer number: " << fbo_ << std::endl;
  std::cout << "Size of Framebuffer: " << window_width << "x" <<
  window_height << std::endl;

  // Create the gbuffer textures
  glGenTextures(ARRAY_SIZE_IN_ELEMENTS(textures_), textures_);
  glGenTextures(1, &depth_texture_);
  glGenTextures(1, &final_texture_);

  std::cout << "Generating " << ARRAY_SIZE_IN_ELEMENTS(textures_) <<
  " textures" << std::endl;
  for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(textures_); ++i) {
    glBindTexture(GL_TEXTURE_2D, textures_[ i ]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window_width, window_height, 0,
                 GL_RGB, GL_FLOAT, NULL);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D, textures_[ i ], 0);
  }
  std::cout << "Generated all textures" << std::endl;

  // depth
  glBindTexture(GL_TEXTURE_2D, depth_texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, window_width,
               window_height, 0, GL_DEPTH_STENCIL,
               GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                         GL_TEXTURE_2D, depth_texture_, 0);

  // final
  glBindTexture(GL_TEXTURE_2D, final_texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0,
               GL_RGB, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4,
                         GL_TEXTURE_2D, final_texture_, 0);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (status != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << K_RED << "FB error in GBuffer.cpp" << K_RESET << std::endl;

    switch (status) {
      case GL_FRAMEBUFFER_UNDEFINED:
        std::cout << "GL_FRAMEBUFFER_UNDEFINED" << std::endl;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" <<
        std::endl;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl;
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        std::cout << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << std::endl;
        break;
      default:
        std::cout << "Unknown error while creation of Framebuffer" << std::endl;
        break;
    }

    return false;
  }

  // restore default FBO
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  std::cout << K_GREEN << "Framebuffer #" << fbo_ << " created successfully " <<
  K_RESET << std::endl;
  return true;
}

void FrameBuffer::StartFrame() {

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
  glDrawBuffer(GL_COLOR_ATTACHMENT4);
  glClear(GL_COLOR_BUFFER_BIT);
}

void FrameBuffer::BindForGeometryPass() {

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);

  GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0,
                            GL_COLOR_ATTACHMENT1,
                            GL_COLOR_ATTACHMENT2 };

  glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(draw_buffers), draw_buffers);
}

void FrameBuffer::BindForStencilPass() {

  // must disable the draw buffers
  glDrawBuffer(GL_NONE);
}

void FrameBuffer::BindForLightPass() {

  glDrawBuffer(GL_COLOR_ATTACHMENT4);

  for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(textures_); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D,
                  textures_[ FRAMEBUFFER_TEXTURE_TYPE_POSITION + i ]);
  }
}

void FrameBuffer::BindForFinalPass() {

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
  glReadBuffer(GL_COLOR_ATTACHMENT4);
}

} // namespace oncgl
