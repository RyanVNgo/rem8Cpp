/*  @file   graphics.cpp
 *  @author Ryan V. Ngo
 */

#include "graphics.h"

#include <GLFW/glfw3.h>


Screen::Screen(std::size_t width, std::size_t height) {
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

Screen::~Screen() {
  glDeleteTextures(1, &m_id);
}

void Screen::update(
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

void Screen::draw() const {
  glBindTexture(GL_TEXTURE_2D, m_id);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f); glVertex2f(-1, 1);
  glTexCoord2f(1.0f, 0.0f); glVertex2f(1, 1);
  glTexCoord2f(1.0f, 1.0f); glVertex2f(1, -1);
  glTexCoord2f(0.0f, 1.0f); glVertex2f(-1, -1);
  glEnd();
}

