#ifndef ONCGL_MODEL_MESH_H
#define ONCGL_MODEL_MESH_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "model/objects.h"
#include "shader_program/shader_program.h"

namespace oncgl {

class Mesh {

 public:
  std::vector<Vertex> vertices_;
  std::vector<GLuint> indices_;
  std::vector<Texture> textures_;

  Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
       std::vector<Texture> textures);

  /**
   * Draw all vertices of the mesh with the given program
   *
   * @param program   id of the program to draw with
   */
  void Draw(GLuint program);

 private:
  /*  Render data  */
  GLuint VAO_, VBO_, EBO_;

  void SetupMesh();
};

} // namespace oncgl

#endif // ONCGL_MODEL_MESH_H
