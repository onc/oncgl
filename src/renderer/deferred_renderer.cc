#include "renderer/renderer.h"

namespace oncgl {

DeferredRenderer::DeferredRenderer(float window_width, float window_height) :
    Renderer(window_width, window_height) {

  std::cout << "compile geometry-shaders" << std::endl;
  geometryShaderProgram_ = LoadShaders(
      RESOURCE_DIRS_PREFIX + "../shaders/geometry/geometry_pass.vert",
      RESOURCE_DIRS_PREFIX + "../shaders/geometry/geometry_pass.frag");

  std::cout << "compile pointlight-shaders" << std::endl;
  pointLightShaderProgram_ = LoadShaders(
      RESOURCE_DIRS_PREFIX + "../shaders/light/light_pass.vert",
      RESOURCE_DIRS_PREFIX + "../shaders/light/pointlight_pass.frag");

  std::cout << "compile dirlight-shaders" << std::endl;
  directionalLightShaderProgram_ = LoadShaders(
      RESOURCE_DIRS_PREFIX + "../shaders/light/light_pass.vert",
      RESOURCE_DIRS_PREFIX + "../shaders/light/dirlight_pass.frag");

  std::cout << "compile nulltech-shaders" << std::endl;
  stencilShaderProgram_ = LoadShaders(
      RESOURCE_DIRS_PREFIX + "../shaders/stencil/null_technique.vert",
      RESOURCE_DIRS_PREFIX + "../shaders/stencil/null_technique.frag");

  std::cout << K_GREEN << "compiled all shaders" << K_RESET << std::endl;

  pointLightModel_ = new Model(
      RESOURCE_DIRS_PREFIX + "../objects/shadingObjects/pointLight.obj");
  directionalLightModel_ = new Model(
      RESOURCE_DIRS_PREFIX + "../objects/shadingObjects/dirLight_quad.obj");

  frameBufferObject_ = new FrameBuffer();
  if(!frameBufferObject_->Init(window_width, window_height)) {
    // error while creating framebuffer
    exit(1);
  }
}

void DeferredRenderer::Init(int window_width, int window_height) {

  frameBufferObject_->StartFrame();
}

void DeferredRenderer::RenderGeometryPass(std::vector<Model> models,
                                          Camera camera) {

  geometryShaderProgram_->Use();

  frameBufferObject_->BindForGeometryPass();

  // Only the geometry pass updates the depth buffer
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  geometryShaderProgram_->setUniform("projection", camera.projection());
  geometryShaderProgram_->setUniform("view", camera.view());
  geometryShaderProgram_->setUniform("model", glm::mat4(1.0f));

  for (GLuint i = 0; i < models.size(); i++) {
    models[ i ].Draw(geometryShaderProgram_);
  }

  geometryShaderProgram_->StopUsing();

  // When we get here the depth buffer is already populated and the stencil pass
  // depends on it, but it does not write to it.
  glDepthMask(GL_FALSE);
}

void DeferredRenderer::RenderStencilPass(PointLight point_light,
                                         Camera camera) {

  stencilShaderProgram_->Use();

  frameBufferObject_->BindForStencilPass();

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glClear(GL_STENCIL_BUFFER_BIT);

  // We need the stencil test to be enabled but we want it
  // to succeed always. Only the depth test matters.
  glStencilFunc(GL_ALWAYS, 0, 0);

  glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
  glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

  glm::mat4 model =
      glm::translate(glm::mat4(1.0f), point_light.position) *
      glm::scale(glm::mat4(1.0f), glm::vec3(point_light.CalcBoundingSphere())) *
      glm::mat4(1.0f);

  stencilShaderProgram_->setUniform("model", model);
  stencilShaderProgram_->setUniform("projection", camera.projection());
  stencilShaderProgram_->setUniform("view", camera.view());

  pointLightModel_->Draw(stencilShaderProgram_);

  stencilShaderProgram_->StopUsing();
}

void DeferredRenderer::RenderPointLightPass(PointLight point_light,
                                            Camera camera) {

  pointLightShaderProgram_->Use();

  frameBufferObject_->BindForLightPass();

  glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  pointLightShaderProgram_->setUniform("screenSize",
                                       glm::vec3(window_width_, window_height_,
                                                 0.0f));

  pointLightShaderProgram_->setUniform("projection", camera.projection());
  pointLightShaderProgram_->setUniform("view", camera.view());

  glm::mat4 model =
      glm::translate(glm::mat4(1.0f), point_light.position) *
      glm::scale(glm::mat4(1.0f), glm::vec3(point_light.CalcBoundingSphere())) *
      glm::mat4(1.0f);

  pointLightShaderProgram_->setUniform("model", model);

  pointLightShaderProgram_->setUniform("pointLight", point_light);
  pointLightShaderProgram_->setUniform("eyePos", camera.position());

  pointLightShaderProgram_
      ->setUniform("gPositionMap", FrameBuffer::FRAMEBUFFER_TEXTURE_TYPE_POSITION);
  pointLightShaderProgram_
      ->setUniform("gColorMap", FrameBuffer::FRAMEBUFFER_TEXTURE_TYPE_DIFFUSE);
  pointLightShaderProgram_
      ->setUniform("gNormalMap", FrameBuffer::FRAMEBUFFER_TEXTURE_TYPE_NORMAL);
  pointLightModel_->Draw(pointLightShaderProgram_);

  glCullFace(GL_BACK);
  glDisable(GL_BLEND);

  pointLightShaderProgram_->StopUsing();
}

void DeferredRenderer::RenderDirectionalLightPass(
    DirectionalLight directional_light, Camera camera) {

  directionalLightShaderProgram_->Use();

  frameBufferObject_->BindForLightPass();

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  directionalLightShaderProgram_->setUniform("screenSize",
                                             glm::vec3(window_width_,
                                                       window_height_, 0.0f));

  directionalLightShaderProgram_->setUniform("projection", camera.projection());
  directionalLightShaderProgram_->setUniform("view", camera.view());


  glm::mat4 model =
      glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f)) *
      glm::rotate(glm::mat4(1.0f), glm::radians<float>(90.0),
                  glm::vec3(1.0f, 0.0f, 0.0f)) *
      glm::mat4(1.0);

  directionalLightShaderProgram_->setUniform("model", model);

  directionalLightShaderProgram_->setUniform("dirLight", directional_light);
  directionalLightShaderProgram_->setUniform("eyePos", camera.position());
  directionalLightShaderProgram_
      ->setUniform("gPositionMap", FrameBuffer::FRAMEBUFFER_TEXTURE_TYPE_POSITION);
  directionalLightShaderProgram_
      ->setUniform("gColorMap", FrameBuffer::FRAMEBUFFER_TEXTURE_TYPE_DIFFUSE);
  directionalLightShaderProgram_
      ->setUniform("gNormalMap", FrameBuffer::FRAMEBUFFER_TEXTURE_TYPE_NORMAL);

  directionalLightModel_->Draw(directionalLightShaderProgram_);

  glDisable(GL_BLEND);

  directionalLightShaderProgram_->StopUsing();
}

void DeferredRenderer::RenderFinalPass() {

  frameBufferObject_->BindForFinalPass();
  glBlitFramebuffer(0, 0, window_width_, window_height_,
                    0, 0, window_width_, window_height_,
                    GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

Program *Renderer::LoadShaders(std::string vertex_shader,
                               std::string fragment_shader) {
  std::vector<Shader> shaders;
  shaders.push_back(Shader::ShaderFromFile(vertex_shader, GL_VERTEX_SHADER));
  shaders
      .push_back(Shader::ShaderFromFile(fragment_shader, GL_FRAGMENT_SHADER));
  return new Program(shaders);
}

} // namespace oncgl
