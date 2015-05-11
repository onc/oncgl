#ifndef ONCGL_SHADER_PROGRAM_SHADER_H
#define ONCGL_SHADER_PROGRAM_SHADER_H

#include <string>

#include <GL/glew.h>

namespace oncgl {

/**
 * Represents a compiled Shader
 */
class Shader {
 public:

  /**
   * Creates a shader from a given file
   *
   * @param  filePath          The path to the shaderfile
   * @param  shaderType        Type of shader. For example GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
   * @throws std::exception    On error
   */
  static Shader
  ShaderFromFile(const std::string &file_path, GLenum shader_type);

  /**
   * Creates a shader from a string of shader source code.
   *
   * @param  shaderCode        The source code for the shader.
   * @param  shaderType        Type of shader. For example GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
   *
   * @throws std::exception    On error
   */
  Shader(const std::string &shader_code, GLenum shaderType);

  /**
   * Get shader object id from glCreateShader
   * @returns shaderID
   */
  GLuint object() const;

  // onmo::Shader objects can be copied and assigned because they are reference counted
  // like a shared pointer
  Shader(const Shader &other);

  Shader &operator=(const Shader &other);

  ~Shader();

 private:
  GLuint object_;
  unsigned *refCount_;

  void _retain();

  void _release();
};

} // namespace oncgl

#endif // ONCGL_SHADER_PROGRAM_SHADER_H
