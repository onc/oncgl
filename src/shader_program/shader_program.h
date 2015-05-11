#ifndef ONCGL_SHADER_PROGRAM_SHADER_PROGRAM_H
#define ONCGL_SHADER_PROGRAM_SHADER_PROGRAM_H

#include <vector>
#include <stdexcept>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "light/lights.h"
#include "shader_program/shader.h"
#include "shader_program/shader_program.h"

namespace oncgl {

class Program {
 public:

  /**
   * Create a program by linking list of Shaders
   *
   * @param  shaders           Shaders to link
   * @throws std::exception    on error
   */
  Program(const std::vector<Shader> &shaders);

  ~Program();

  /**
   * Get the program's object id from glCreateProgram
   *
   * @result programID
   */
  GLuint object() const;

  /**
   * Get the attribute index for a given name from glGetAttribLocation
   *
   * @param  attribName    Attribute name
   * @result attributeIndex
   */
  GLint attrib(const GLchar *attribName) const;

  void Use() const;

  bool IsInUse() const;

  void StopUsing() const;

  /**
   * Get the uniform index for a given name from glGetUniformLocation
   *
   * @param  uniformName    Name of unform
   * @result uniformIndex
   */
  GLint uniform(const GLchar *uniformName) const;

  /**
   * Setters for attribute and uniform variables.
   * These are convenience methods for the glVertexAttrib* and glUniform* functions.
   */
#define _TDOGL_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(OGL_TYPE)               \
  void setAttrib(const GLchar* attribName, OGL_TYPE v0);                \
  void setAttrib(const GLchar* attribName, OGL_TYPE v0, OGL_TYPE v1);   \
  void setAttrib(const GLchar* attribName, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2); \
  void setAttrib(const GLchar* attribName, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3); \
                                                                        \
  void setAttrib1v(const GLchar* attribName, const OGL_TYPE* v);        \
  void setAttrib2v(const GLchar* attribName, const OGL_TYPE* v);        \
  void setAttrib3v(const GLchar* attribName, const OGL_TYPE* v);        \
  void setAttrib4v(const GLchar* attribName, const OGL_TYPE* v);        \
                                                                        \
  void setUniform(const GLchar* uniformName, OGL_TYPE v0);              \
  void setUniform(const GLchar* uniformName, OGL_TYPE v0, OGL_TYPE v1); \
  void setUniform(const GLchar* uniformName, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2); \
  void setUniform(const GLchar* uniformName, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3); \
                                                                        \
  void setUniform1v(const GLchar* uniformName, const OGL_TYPE* v, GLsizei count=1); \
  void setUniform2v(const GLchar* uniformName, const OGL_TYPE* v, GLsizei count=1); \
  void setUniform3v(const GLchar* uniformName, const OGL_TYPE* v, GLsizei count=1); \
  void setUniform4v(const GLchar* uniformName, const OGL_TYPE* v, GLsizei count=1); \


  _TDOGL_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLfloat)

  _TDOGL_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLdouble)

  _TDOGL_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLint)

  _TDOGL_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLuint)

  void setUniformMatrix2(const GLchar *uniformName, const GLfloat *v,
                         GLsizei count = 1, GLboolean transpose = GL_FALSE);

  void setUniformMatrix3(const GLchar *uniformName, const GLfloat *v,
                         GLsizei count = 1, GLboolean transpose = GL_FALSE);

  void setUniformMatrix4(const GLchar *uniformName, const GLfloat *v,
                         GLsizei count = 1, GLboolean transpose = GL_FALSE);

  void setUniform(const GLchar *uniformName, const glm::mat2 &m,
                  GLboolean transpose = GL_FALSE);

  void setUniform(const GLchar *uniformName, const glm::mat3 &m,
                  GLboolean transpose = GL_FALSE);

  void setUniform(const GLchar *uniformName, const glm::mat4 &m,
                  GLboolean transpose = GL_FALSE);

  void setUniform(const GLchar *uniformName, const glm::vec3 &v);

  void setUniform(const GLchar *uniformName, const glm::vec4 &v);

  void setUniform(const GLchar *uniformName, const Light &light);

  void setUniform(const GLchar *uniformName,
                  const DirectionalLight &directionalLight);

  void setUniform(const GLchar *uniformName, const PointLight &pointLight);

  void setUniform(const GLchar *uniformName, const unsigned int &TextureUnit);

 private:
  GLuint object_;

  //copying disabled
  Program(const Program &);

  const Program &operator=(const Program &);

  struct {

    GLint color;
    GLint ambient_intensity;
    GLint diffuse_intensity;
    GLint position;
    struct {

      GLint constant;
      GLint linear;
      GLint exponent;
    } attenuation;
  } pointLightLocation_;

  struct {

    GLint color;
    GLint ambient_intensity;
    GLint diffuse_intensity;
    GLint direction;
  } directionalLightLocation_;
};

} // namespace oncgl

#endif // ONCGL_SHADER_PROGRAM_SHADER_PROGRAM_H
