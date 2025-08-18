/*  @file   graphics.cpp
 *  @author Ryan V. Ngo
 */

#include "graphics.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"


void initialize_screen_texture(GLuint& texture, size_t width, size_t height) {
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
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
}

void update_screen_texture(GLuint texture, size_t width, size_t height, std::vector<unsigned char>& data) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexSubImage2D(
      GL_TEXTURE_2D,
      0, 0, 0,
      width, height,
      GL_RGB,
      GL_UNSIGNED_BYTE,
      data.data()
  );
}

void draw_screen_texture(GLuint texture) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f); glVertex2f(-1, 1);
  glTexCoord2f(1.0f, 0.0f); glVertex2f(1, 1);
  glTexCoord2f(1.0f, 1.0f); glVertex2f(1, -1);
  glTexCoord2f(0.0f, 1.0f); glVertex2f(-1, -1);
  glEnd();
}

