/*  @file   graphics.h
 *  @author Ryan V. Ngo
 */

#pragma once

#include <GL/glew.h>


//---------------------------------------------------
// General GL Calls
//---------------------------------------------------

void update_viewport(std::size_t width, std::size_t height);
void clear();

//---------------------------------------------------
// Screen
//---------------------------------------------------

class Screen {
  public:
    Screen(std::size_t width, std::size_t height);
    ~Screen();

    void update(
        std::size_t x_offset,
        std::size_t y_offset,
        std::size_t width,
        std::size_t height,
        const uint8_t* data
    );

    void draw() const;

    Screen(const Screen& other) = delete;
    Screen(Screen&& other) = delete;
    Screen& operator=(const Screen& other) = delete;
    Screen& operator=(Screen&& other) = delete;

  private:
    GLuint m_id{0};

};

