/*  @file   graphics.cpp
 *  @author Ryan V. Ngo
 */

#include "graphics.h"

#include <GLFW/glfw3.h>


//---------------------------------------------------
// Texture
//---------------------------------------------------

Texture::Texture(std::size_t width, std::size_t height) {
  glGenTextures(1, &m_id);
  glBindTexture(GL_TEXTURE_2D, m_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(
      GL_TEXTURE_2D, 
      0, 
      GL_RGB32F,
      width, 
      height, 
      0, 
      GL_RGB, 
      GL_UNSIGNED_BYTE, 
      nullptr
  ); 
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
  glDeleteTextures(1, &m_id);
}

void Texture::update(
    std::size_t x_offset,
    std::size_t y_offset,
    std::size_t width,
    std::size_t height,
    const uint8_t* data
) {
  glBindTexture(GL_TEXTURE_2D, m_id);
  glTexSubImage2D(
      GL_TEXTURE_2D,
      0, 
      x_offset, y_offset,
      width, height,
      GL_RGB,
      GL_UNSIGNED_BYTE,
      data
  );
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() const {
  glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const {
  glBindTexture(GL_TEXTURE_2D, 0);
}


//---------------------------------------------------
// General GL Calls
//---------------------------------------------------

void update_viewport(std::size_t width, std::size_t height) {
  glViewport(0, 0, width, height);
}

void clear() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void draw_texture(const Texture& tex) {
  tex.bind();
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f); glVertex2f(-1, 1);
  glTexCoord2f(1.0f, 0.0f); glVertex2f(1, 1);
  glTexCoord2f(1.0f, 1.0f); glVertex2f(1, -1);
  glTexCoord2f(0.0f, 1.0f); glVertex2f(-1, -1);
  glEnd();
  tex.unbind();
}

