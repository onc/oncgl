#include "model/model.h"

namespace oncgl {

Model::Model(std::string path, glm::mat4 model_matrix) :
    path_(path),
    model_matrix_(model_matrix) {

  LoadModel(path_);
}

void Model::Draw(Program *program) {

  // std::cout << _meshes.size() << std::endl;
  for (GLuint i = 0; i < meshes_.size(); i++) {
    meshes_[ i ].Draw(program->object());
  }
}

void Model::set_model_matrix(glm::mat4 matrix) {
  model_matrix_ = matrix;
}

glm::mat4 Model::model_matrix() const {
  return model_matrix_;
}

void Model::LoadModel(std::string path) {

  // Read file via ASSIMP
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(path,
                                           aiProcess_Triangulate |
                                           aiProcess_FlipUVs |
                                           aiProcess_CalcTangentSpace);
  // Check for errors
  if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << K_RED << "ERROR::ASSIMP:: " << importer.GetErrorString() <<
        K_RESET << std::endl;
    return;
  }

  // Retrieve the directory path of the filepath
  directory_ = path.substr(0, path.find_last_of('/'));

  // Process ASSIMP's root node recursively
  ProcessNode(scene->mRootNode, scene);
  // std::cout << _meshes.size() << std::endl;
}

void Model::ProcessNode(aiNode *node, const aiScene *scene) {

  // Process each mesh located at the current node
  for (GLuint i = 0; i < node->mNumMeshes; i++) {
    // The node object only contains indices to index the actual objects in the scene.
    // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
    aiMesh *mesh = scene->mMeshes[ node->mMeshes[ i ]];
    meshes_.push_back(ProcessMesh(mesh, scene));
  }
  // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
  for (GLuint i = 0; i < node->mNumChildren; i++) {
    ProcessNode(node->mChildren[ i ], scene);
  }
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
  // Data to fill
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;

  // Walk through each of the mesh's vertices
  for (GLuint i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    // We declare a placeholder vector since assimp uses its own vector class that doesn't directly
    // convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
    glm::vec3 vector;
    // Positions
    vector.x = mesh->mVertices[ i ].x;
    vector.y = mesh->mVertices[ i ].y;
    vector.z = mesh->mVertices[ i ].z;
    vertex.position = vector;
    // Normals
    vector.x = mesh->mNormals[ i ].x;
    vector.y = mesh->mNormals[ i ].y;
    vector.z = mesh->mNormals[ i ].z;
    vertex.normal = vector;
    // Texture Coordinates
    // Does the mesh contain texture coordinates?
    if (mesh->mTextureCoords[ 0 ]) {
      glm::vec2 vec;
      // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
      // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
      vec.x = mesh->mTextureCoords[ 0 ][ i ].x;
      vec.y = mesh->mTextureCoords[ 0 ][ i ].y;
      vertex.tex_coords = vec;

      glm::vec3 tang;
      tang.x = mesh->mTangents[ i ].x;
      tang.y = mesh->mTangents[ i ].y;
      tang.z = mesh->mTangents[ i ].z;
      vertex.tangent = tang;

      glm::vec3 bi_tang;
      bi_tang.x = mesh->mBitangents[ i ].x;
      bi_tang.y = mesh->mBitangents[ i ].y;
      bi_tang.z = mesh->mBitangents[ i ].z;
      vertex.bi_tangent = bi_tang;
    } else {
      vertex.tex_coords = glm::vec2(0.0f, 0.0f);
    }

    vertices.push_back(vertex);
  }
  // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
  for (GLuint i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[ i ];
    // Retrieve all indices of the face and store them in the indices vector
    for (GLuint j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[ j ]);
    }
  }
  // Process materials
  aiMaterial *material = scene->mMaterials[ mesh->mMaterialIndex ];
  // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
  // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
  // Same applies to other texture as the following list summarizes:
  // Diffuse: texture_diffuseN
  // Specular: texture_specularN
  // Normal: texture_normalN

  // 1. Diffuse maps
  std::vector<Texture>
      diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE,
                                         "texture_diffuse");
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
  // 2. Specular maps
  std::vector<Texture>
      specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR,
                                          "texture_specular");
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  // 3. normals for bump maps
  std::vector<Texture>
      normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT,
                                        "texture_normals");
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  // Return a mesh object created from the extracted mesh data
  return Mesh(vertices, indices, textures);
}

// Checks all material textures of a given type and loads the textures if they're not loaded yet.
// The required info is returned as a Texture struct.
std::vector<Texture> Model::LoadMaterialTextures(aiMaterial *mat,
                                                 aiTextureType type,
                                                 std::string type_name) {

  std::vector<Texture> textures;
  for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    GLboolean skip = false;
    for (GLuint j = 0; j < textures_loaded_.size(); j++) {
      if (textures_loaded_[ j ].path == str) {
        textures.push_back(textures_loaded_[ j ]);
        // A texture with the same filepath has already been loaded,
        // continue to next one. (optimization)
        skip = true;
        break;
      }
    }
    if (!skip) {   // If texture hasn't been loaded already, load it
      Texture texture;
      texture.id = TextureFromFile(str.C_Str(), directory_);
      texture.type = type_name;
      texture.path = str;
      textures.push_back(texture);
      // Store it as texture loaded for entire model, to ensure we won't
      // unnecesery load duplicate textures.
      textures_loaded_.push_back(texture);
    }
  }
  return textures;
}

GLint Model::TextureFromFile(const char *path, std::string directory) {
  //Generate texture ID and load texture data
  std::string filename = std::string(path);
  filename = directory + '/' + filename;
  GLuint textureID;
  glGenTextures(1, &textureID);
  int width, height;
  unsigned char *image = SOIL_load_image(filename.c_str(), &width, &height, 0,
                                         SOIL_LOAD_RGB);
  // Assign texture to ID
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  SOIL_free_image_data(image);
  return textureID;
}

} // namespace oncgl
