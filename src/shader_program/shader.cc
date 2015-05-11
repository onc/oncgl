#include "shader_program/shader.h"

#include <stdexcept>
#include <fstream>
#include <cassert>
#include <sstream>

namespace oncgl {

Shader::Shader(const std::string &shader_code, GLenum shader_type) :
    object_(0), refCount_(NULL) {

  //create the shader object
  object_ = glCreateShader(shader_type);
  if (object_ == 0) {
    throw std::runtime_error("glCreateShader failed");
  }

  //set the source code
  const char *code = shader_code.c_str();
  glShaderSource(object_, 1, (const GLchar **) &code, NULL);

  //compile
  glCompileShader(object_);

  //throw exception if compile error occurred
  GLint status;
  glGetShaderiv(object_, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    std::string msg("Compile failure in shader:\n");

    GLint infoLogLength;
    glGetShaderiv(object_, GL_INFO_LOG_LENGTH, &infoLogLength);
    char *strInfoLog = new char[infoLogLength + 1];
    glGetShaderInfoLog(object_, infoLogLength, NULL, strInfoLog);
    msg += strInfoLog;
    delete[] strInfoLog;

    glDeleteShader(object_);
    object_ = 0;
    throw std::runtime_error(msg);
  }

  refCount_ = new unsigned;
  *refCount_ = 1;
}

Shader::Shader(const Shader &other) :
    object_(other.object_),
    refCount_(other.refCount_) {
  _retain();
}

Shader::~Shader() {
  //_refCount will be NULL if constructor failed and threw an exception
  if (refCount_) {
    _release();
  }
}

GLuint Shader::object() const {
  return object_;
}

Shader &Shader::operator=(const Shader &other) {
  _release();
  object_ = other.object_;
  refCount_ = other.refCount_;
  _retain();
  return *this;
}

Shader Shader::ShaderFromFile(const std::string &file_path,
                              GLenum shader_type) {
  //open file
  std::ifstream f;
  f.open(file_path.c_str(), std::ios::in | std::ios::binary);
  if (!f.is_open()) {
    throw std::runtime_error(std::string("Failed to open file: ") + file_path);
  }

  //read whole file into stringstream buffer
  std::stringstream buffer;
  buffer << f.rdbuf();

  //return new shader
  Shader shader(buffer.str(), shader_type);
  return shader;
}

void Shader::_retain() {
  assert(refCount_);
  *refCount_ += 1;
}

void Shader::_release() {
  assert(refCount_ && *refCount_ > 0);
  *refCount_ -= 1;
  if (*refCount_ == 0) {
    glDeleteShader(object_);
    object_ = 0;
    delete refCount_;
    refCount_ = NULL;
  }
}

}
