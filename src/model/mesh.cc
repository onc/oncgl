#include "mesh.h"

namespace oncgl {

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
           std::vector<Texture> textures) :
    vertices_(vertices), indices_(indices), textures_(textures) {

  SetupMesh();
}

void Mesh::SetupMesh() {

  glGenVertexArrays(1, &VAO_);
  glGenBuffers(1, &VBO_);
  glGenBuffers(1, &EBO_);

  glBindVertexArray(VAO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);

  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
               &vertices_[ 0 ], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint),
               &indices_[ 0 ], GL_STATIC_DRAW);

  // vertex position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *) 0);

  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *) offsetof(Vertex, normal));

  // vertex textures coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FLOAT, sizeof(Vertex),
                        (GLvoid *) offsetof(Vertex, tex_coords));

  // tangents
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FLOAT, sizeof(Vertex),
                        (GLvoid *) offsetof(Vertex, tangent));

  // bitangents
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FLOAT, sizeof(Vertex),
                        (GLvoid *) offsetof(Vertex, bi_tangent));

  glBindVertexArray(0);
}

void Mesh::Draw(GLuint program) {

  // Bind appropriate textures
  GLuint diffuse_nr = 1;
  GLuint specular_nr = 1;
  GLuint normal_nr = 1;

  for (GLuint i = 0; i < this->textures_.size(); i++) {

    glActiveTexture(
        GL_TEXTURE0 + i); // Active proper texture unit before binding
    // Retrieve texture number (the N in diffuse_textureN)
    std::stringstream ss;
    std::string number;
    std::string name = this->textures_[ i ].type;
    if (name == "texture_diffuse") {
      ss << diffuse_nr++; // Transfer GLuint to streams
    } else if (name == "texture_specular") {
      ss << specular_nr++; // Transfer GLuint to stream
    } else if (name == "texture_normals") {
      ss << normal_nr++;
    }
    number = ss.str();
    // Now set the sampler to the correct texture unit
    glUniform1f(glGetUniformLocation(program, (name + number).c_str()), i);
    // And finally bind the texture
    glBindTexture(GL_TEXTURE_2D, this->textures_[ i ].id);
  }

  // Also set each mesh's shininess property to a default value (if you want
  // you could extend this to another mesh property and possibly change this value)
  glUniform1f(glGetUniformLocation(program, "material.shininess"), 16.0f);

  // Draw mesh
  glBindVertexArray(VAO_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // Always good practice to set everything back to defaults once configured.
  for (GLuint i = 0; i < textures_.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

} // namespace oncgl
