/*  @file   window.h
 *  @brief  Window method interface.
 *  @author Ryan V. Ngo
 */

#pragma once

#include <GLFW/glfw3.h>

#include <cstddef>


//---------------------------------------------------
// ApplicationWindow
//---------------------------------------------------

class ApplicationWindow {
  public:
    ApplicationWindow(const char* name, std::size_t width, std::size_t height);
    ~ApplicationWindow();

    bool valid();
    void set_vsync(bool mode);
    void make_current_context();
    void swap_buffers();
    bool should_close();
    bool is_key_pressed(int glfw_key);
    void frame_buff_size(std::size_t& width, std::size_t& height) const;

    GLFWwindow* window() const { return m_window; }

    ApplicationWindow(const ApplicationWindow& other) = delete;
    ApplicationWindow(ApplicationWindow&& other) = delete;
    ApplicationWindow& operator=(const ApplicationWindow& other) = delete;
    ApplicationWindow& operator=(ApplicationWindow&& other) = delete;

  private:
    GLFWwindow* m_window;

};

