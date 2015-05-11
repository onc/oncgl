#ifndef  ONCGL_MODEL_OBJECT_H
#define ONCGL_MODEL_OBJECT_H

#include <string>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <assimp/Importer.hpp>

namespace oncgl {

struct Vertex {

  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coords;

  glm::vec3 tangent;
  glm::vec3 bi_tangent;
};

struct Texture {

  GLuint id;
  std::string type;
  aiString path;
};

} // namespace oncgl

#endif // ONCGL_MODEL_OBJECT_H
