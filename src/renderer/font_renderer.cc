#include "renderer/renderer.h"

using namespace oncgl;

FontRenderer::FontRenderer(std::string font_path, unsigned int font_size,
                           float window_width, float window_height) :
    Renderer(window_width, window_height) {

  std::cout << "compile font-shaders" << std::endl;
  fontShaderProgram_ = LoadShaders(
      RESOURCE_DIRS_PREFIX + "../shaders/font/font.vert",
      RESOURCE_DIRS_PREFIX + "../shaders/font/font.frag");
  std::cout << "font-shaders compiled successfully" << std::endl;

  if (FT_Init_FreeType(&ft_)) {
    std::cout << K_RED << "ERROR: Could not init FreeType" << K_RESET <<
        std::endl;
  }

  if (FT_New_Face(ft_, font_path.c_str(), 0, &face_)) {
    std::cout << K_RED << "ERROR: Failed to load font" << K_RESET << std::endl;
  }

  FT_Set_Pixel_Sizes(face_, 0, font_size);

  // Disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (GLubyte c = 0; c < 128; c++) {

    // Load character glyph
    if (FT_Load_Char(face_, c, FT_LOAD_RENDER)) {
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
      continue;
    }

    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        face_->glyph->bitmap.width,
        face_->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        face_->glyph->bitmap.buffer
                 );

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Now store character for later use
    Character character = {
      texture,
      glm::ivec2(face_->glyph->bitmap.width, face_->glyph->bitmap.rows),
      glm::ivec2(face_->glyph->bitmap_left, face_->glyph->bitmap_top),
      face_->glyph->advance.x
    };
    characters_.insert(std::pair<GLchar, Character>(c, character));
  }

  FT_Done_Face(face_);
  FT_Done_FreeType(ft_);
}

void FontRenderer::CreateProjectionMatrix(float width, float height) {

  projection_ = glm::ortho(0.0f, width, 0.0f, height);
}

void FontRenderer::Init(float width, float height) {

  CreateProjectionMatrix(width, height);

  glGenVertexArrays(1, &VAO_);
  glGenBuffers(1, &VBO_);

  glBindVertexArray(VAO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);

  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void FontRenderer::RenderText(std::string text, GLfloat x, GLfloat y,
                              GLfloat scale, glm::vec3 color) {

  bool isCullEnabled = glIsEnabled(GL_CULL_FACE);

  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  fontShaderProgram_->Use();
  fontShaderProgram_->setUniform("textColor", color);
  fontShaderProgram_->setUniform("projection", projection_);

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO_);

  // Iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {

    Character ch = characters_[ *c ];

    GLfloat xpos = x + ch.bearing.x * scale;
    GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

    GLfloat w = ch.size.x * scale;
    GLfloat h = ch.size.y * scale;
    // Update VBO for each character
    GLfloat vertices[6][4] = {
      { xpos,     ypos + h, 0.0, 0.0 },
      { xpos,     ypos,     0.0, 1.0 },
      { xpos + w, ypos,     1.0, 1.0 },

      { xpos,     ypos + h, 0.0, 0.0 },
      { xpos + w, ypos,     1.0, 1.0 },
      { xpos + w, ypos + h, 1.0, 0.0 }
    };
    // Render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.texture_id);
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    x += (ch.advance >> 6) *
        scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

  if (isCullEnabled) {
    glEnable(GL_CULL_FACE);
  }

  glDisable(GL_BLEND);
}
