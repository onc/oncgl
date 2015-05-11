#include "shader_program/shader_program.h"

using namespace oncgl;

Program::Program(const std::vector<Shader>& shaders):
    object_(0) {

  if(shaders.size() <= 0) {
    throw std::runtime_error("No shaders were provided to create the program");
  }

  object_ = glCreateProgram();
  if(object_ == 0) {
    throw std::runtime_error("glCreateProgram failed");
  }

  // attach all the shaders
  for(unsigned i = 0; i < shaders.size(); ++i) {
    glAttachShader(object_, shaders[i].object());
  }

  //link the shaders together
  glLinkProgram(object_);

  // detach all the shaders
  for(unsigned i = 0; i < shaders.size(); ++i) {
    glDetachShader(object_, shaders[i].object());
  }

  // throw exception if linking failed
  GLint status;
  glGetProgramiv(object_, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    std::string msg("Program linking failure: ");

    GLint info_log_length;
    glGetProgramiv(object_, GL_INFO_LOG_LENGTH, &info_log_length);
    char*str_info_log = new char[info_log_length + 1];
    glGetProgramInfoLog(object_, info_log_length, NULL, str_info_log);
    msg += str_info_log;
    delete[] str_info_log;

    glDeleteProgram(object_); object_ = 0;
    throw std::runtime_error(msg);
  }
}

Program::~Program() {
  //might be 0 if ctor fails by throwing exception
  if(object_ != 0) glDeleteProgram(object_);
}

GLuint Program::object() const {
  return object_;
}

void Program::Use() const {
  glUseProgram(object_);
}

bool Program::IsInUse() const {
  GLint currentProgram = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  return (currentProgram == (GLint) object_);
}

void Program::StopUsing() const {
  assert(IsInUse());
  glUseProgram(0);
}

GLint Program::attrib(const GLchar *attribName) const {
  if(!attribName) {
    throw std::runtime_error("attribName was NULL");
  }

  GLint attrib = glGetAttribLocation(object_, attribName);
  if(attrib == -1) {
    throw std::runtime_error(std::string("Program attribute not found: ") + attribName);
  }
  return attrib;
}

GLint Program::uniform(const GLchar* uniformName) const {
  if(!uniformName) {
    throw std::runtime_error("uniformName was NULL");
  }

  GLint uniform = glGetUniformLocation(object_, uniformName);
  if(uniform == -1) {
    throw std::runtime_error(std::string("Program uniform not found: ") + uniformName);
  }
  return uniform;
}

#define ATTRIB_N_UNIFORM_SETTERS(OGL_TYPE, TYPE_PREFIX, TYPE_SUFFIX)    \
                                                                        \
  void Program::setAttrib(const GLchar* name, OGL_TYPE v0)              \
  { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 1 ## TYPE_SUFFIX (attrib(name), v0); } \
  void Program::setAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1) \
  { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 2 ## TYPE_SUFFIX (attrib(name), v0, v1); } \
  void Program::setAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2) \
  { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 3 ## TYPE_SUFFIX (attrib(name), v0, v1, v2); } \
  void Program::setAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3) \
  { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 4 ## TYPE_SUFFIX (attrib(name), v0, v1, v2, v3); } \
                                                                        \
  void Program::setAttrib1v(const GLchar* name, const OGL_TYPE* v)      \
  { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 1 ## TYPE_SUFFIX ## v (attrib(name), v); } \
  void Program::setAttrib2v(const GLchar* name, const OGL_TYPE* v)      \
  { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 2 ## TYPE_SUFFIX ## v (attrib(name), v); } \
  void Program::setAttrib3v(const GLchar* name, const OGL_TYPE* v)      \
  { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 3 ## TYPE_SUFFIX ## v (attrib(name), v); } \
  void Program::setAttrib4v(const GLchar* name, const OGL_TYPE* v)      \
  { assert(IsInUse()); glVertexAttrib ## TYPE_PREFIX ## 4 ## TYPE_SUFFIX ## v (attrib(name), v); } \
                                                                        \
  void Program::setUniform(const GLchar* name, OGL_TYPE v0)             \
  { assert(IsInUse()); glUniform1 ## TYPE_SUFFIX (uniform(name), v0); } \
  void Program::setUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1) \
  { assert(IsInUse()); glUniform2 ## TYPE_SUFFIX (uniform(name), v0, v1); } \
  void Program::setUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2) \
  { assert(IsInUse()); glUniform3 ## TYPE_SUFFIX (uniform(name), v0, v1, v2); } \
  void Program::setUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3) \
  { assert(IsInUse()); glUniform4 ## TYPE_SUFFIX (uniform(name), v0, v1, v2, v3); } \
                                                                        \
  void Program::setUniform1v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
  { assert(IsInUse()); glUniform1 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
  void Program::setUniform2v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
  { assert(IsInUse()); glUniform2 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
  void Program::setUniform3v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
  { assert(IsInUse()); glUniform3 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
  void Program::setUniform4v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
  { assert(IsInUse()); glUniform4 ## TYPE_SUFFIX ## v (uniform(name), count, v); }

ATTRIB_N_UNIFORM_SETTERS(GLfloat, , f);
ATTRIB_N_UNIFORM_SETTERS(GLdouble, , d);
ATTRIB_N_UNIFORM_SETTERS(GLint, I, i);
ATTRIB_N_UNIFORM_SETTERS(GLuint, I, ui);

void Program::setUniformMatrix2(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
  assert(IsInUse());
  glUniformMatrix2fv(uniform(name), count, transpose, v);
}

void Program::setUniformMatrix3(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
  assert(IsInUse());
  glUniformMatrix3fv(uniform(name), count, transpose, v);
}

void Program::setUniformMatrix4(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
  assert(IsInUse());
  glUniformMatrix4fv(uniform(name), count, transpose, v);
}

void Program::setUniform(const GLchar* name, const glm::mat2& m, GLboolean transpose) {
  assert(IsInUse());
  glUniformMatrix2fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void Program::setUniform(const GLchar* name, const glm::mat3& m, GLboolean transpose) {
  assert(IsInUse());
  glUniformMatrix3fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void Program::setUniform(const GLchar* name, const glm::mat4& m, GLboolean transpose) {
  assert(IsInUse());
  glUniformMatrix4fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void Program::setUniform(const GLchar* uniformName, const glm::vec3& v) {
  setUniform3v(uniformName, glm::value_ptr(v));
}

void Program::setUniform(const GLchar* uniformName, const glm::vec4& v) {
  setUniform4v(uniformName, glm::value_ptr(v));
}

void Program::setUniform(const GLchar* uniformName, const PointLight& light) {

  std::string uniName(uniformName);

  pointLightLocation_.position = glGetUniformLocation(object_, (uniName + ".position").c_str());

  pointLightLocation_.color = glGetUniformLocation(object_, (uniName + ".light.color").c_str());
  pointLightLocation_.ambient_intensity = glGetUniformLocation(object_, (uniName + ".light.ambient_intensity").c_str());
  pointLightLocation_.diffuse_intensity = glGetUniformLocation(object_, (uniName + ".light.diffuse_intensity").c_str());

  pointLightLocation_.attenuation.constant = glGetUniformLocation(object_, (uniName + ".atten.constant").c_str());
  pointLightLocation_.attenuation.linear = glGetUniformLocation(object_, (uniName + ".atten.linear").c_str());
  pointLightLocation_.attenuation.exponent = glGetUniformLocation(object_, (uniName + ".atten.exponent").c_str());

  glUniform3f(pointLightLocation_.position, light.position.x, light.position.y, light.position.z);

  glUniform3f(pointLightLocation_.color, light.color.r, light.color.g, light.color.b);
  glUniform1f(pointLightLocation_.ambient_intensity, light.ambient_intensity);
  glUniform1f(pointLightLocation_.diffuse_intensity, light.diffuse_intensity);

  glUniform1f(pointLightLocation_.attenuation.constant, light.attenuation.constant);
  glUniform1f(pointLightLocation_.attenuation.linear, light.attenuation.linear);
  glUniform1f(pointLightLocation_.attenuation.exponent, light.attenuation.exp);
}

void Program::setUniform(const GLchar* uniformName, const DirectionalLight& light) {

  std::string uniName(uniformName);

  directionalLightLocation_.direction = glGetUniformLocation(object_, (uniName + ".direction").c_str());

  directionalLightLocation_.color = glGetUniformLocation(object_, (uniName + ".light.color").c_str());
  directionalLightLocation_.ambient_intensity = glGetUniformLocation(object_, (uniName + ".light.ambient_intensity").c_str());
  directionalLightLocation_.diffuse_intensity = glGetUniformLocation(object_, (uniName + ".light.diffuse_intensity").c_str());

  glUniform3f(directionalLightLocation_.direction, light.direction.x, light.direction.y, light.direction.z);

  glUniform3f(directionalLightLocation_.color, light.color.r, light.color.g, light.color.b);
  glUniform1f(directionalLightLocation_.ambient_intensity, light.ambient_intensity);
  glUniform1f(directionalLightLocation_.diffuse_intensity, light.diffuse_intensity);
}

void Program::setUniform(const GLchar* uniformName, const unsigned int& TextureUnit) {

  glUniform1i(uniform(uniformName), TextureUnit);
}
