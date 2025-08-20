/*  @file   graphics.h
 *  @author Ryan V. Ngo
 */

#pragma once

#include <GL/glew.h>


//---------------------------------------------------
// Texture
//---------------------------------------------------

class Texture {
  public:
    Texture(std::size_t width, std::size_t height);
    ~Texture();

    void update(
        std::size_t x_offset,
        std::size_t y_offset,
        std::size_t width,
        std::size_t height,
        const uint8_t* data
    );

    void bind() const;
    void unbind() const;

    Texture(const Texture& other) = delete;
    Texture(Texture&& other) = delete;
    Texture& operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) = delete;

  private:
    GLuint m_id{0};

};


//---------------------------------------------------
// General GL Calls
//---------------------------------------------------

void update_viewport(std::size_t width, std::size_t height);

void clear();

void draw_texture(const Texture& tex);


