/*  @file   window.cpp
 *  @brief  Window implementation.
 *  @author Ryan V. Ngo
 */

#include "window.h"

#include <iostream>


//---------------------------------------------------
// General GLFW helper methods
//---------------------------------------------------

bool initialize_glfw() {
  if (!glfwInit()) { 
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false; 
  }
  return true;
}

void terminate_glfw() {
  glfwTerminate();
}


//---------------------------------------------------
// ApplicationWindow
//---------------------------------------------------

ApplicationWindow::ApplicationWindow(const char* name, std::size_t width, std::size_t height) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  m_window = glfwCreateWindow(width, height, name, NULL, NULL);
  if (!m_window) {
    glfwTerminate();
    std::cerr << "Failed to create window" << std::endl;
    return;
  }
  return;
}

ApplicationWindow::~ApplicationWindow() {
  glfwDestroyWindow(m_window);
}

bool ApplicationWindow::valid() {
  if (!m_window) {
    return false;
  }
  return true;
}

void ApplicationWindow::set_vsync(bool mode) {
  if (mode) {
    glfwSwapInterval(1);
    return;
  }
  glfwSwapInterval(0);
}

void ApplicationWindow::make_current_context() {
  glfwMakeContextCurrent(m_window);
}

void ApplicationWindow::swap_buffers() {
  glfwSwapBuffers(m_window);
}

bool ApplicationWindow::should_close() {
  return glfwWindowShouldClose(m_window);
}

bool ApplicationWindow::is_key_pressed(int glfw_key) {
  return glfwGetKey(m_window, glfw_key) == GLFW_PRESS;
}

