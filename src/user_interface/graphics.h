/*  @file   graphics.h
 *  @author Ryan V. Ngo
 */

#pragma once

#include <GL/glew.h>
#include <vector>


void initialize_screen_texture(GLuint& texture, size_t width, size_t height);
void update_screen_texture(GLuint texture, size_t width, size_t height, std::vector<unsigned char>& data);
void draw_screen_texture(GLuint texture);

