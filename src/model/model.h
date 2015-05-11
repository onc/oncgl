#ifndef  ONCGL_MODEL_MODEL_H
#define ONCGL_MODEL_MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL/SOIL.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader_program/shader_program.h"
#include "model/mesh.h"

#include "misc/constants.h"

namespace oncgl {

class Model {

 public:
  Model(std::string path, glm::mat4 model_matrix = glm::mat4(1.0f));

  /**
   * Draw the Model with the given program
   *
   * @param program   Program to draw the model with
   */
  void Draw(Program *program);

  void set_model_matrix(glm::mat4 matrix);

  glm::mat4 model_matrix() const;

 private:
  std::vector<Mesh> meshes_;
  std::string directory_;
  std::string path_;
  std::vector<Texture> textures_loaded_;

  glm::mat4 model_matrix_;

  void LoadModel(std::string path);

  void ProcessNode(aiNode *node, const aiScene *scene);

  Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

  std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            std::string type_name);

  GLint TextureFromFile(const char *path, std::string directory);
};

} // namespace oncgl

#endif // ONCGL_MODEL_MODEL_H
