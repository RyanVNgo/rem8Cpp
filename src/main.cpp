/*  @file   main.cpp
 *  @brief  Program entry point.
 *  @author Ryan V. Ngo
 */

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

#include "emulator.h"
#include "widgets/control_panel.h"
#include "utilities/file.h"
#include "utilities/instrumentor.h"


void initialize_screen_texture(GLuint& texture, size_t width, size_t height);
void update_screen_texture(GLuint texture, size_t width, size_t height, std::vector<unsigned char>& data);
void draw_screen_texture(GLuint texture);

int main() {
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  if (!glfwInit()) { 
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1; 
  }

  constexpr size_t width = 640;
  constexpr size_t height = 320;
  GLFWwindow* window = glfwCreateWindow(width, height, "rem8C++", NULL, NULL);
  if (!window) {
    glfwTerminate();
    std::cerr << "Failed to create window" << std::endl;
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui::StyleColorsDark();
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  auto emulator = rem8Cpp();
  auto control_panel = ControlPanel(emulator, io);

  size_t screen_width = emulator.width();
  size_t screen_height = emulator.height();
  GLuint screen_texture;
  std::vector<unsigned char> screen_buffer(screen_width * screen_height * 3);
  initialize_screen_texture(screen_texture, screen_width, screen_height);

  // Main loop
  double last_time = 0;
  uint32_t delay_accumulator = 0;
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS ? emulator.set_key('1') : emulator.unset_key('1'); 
    glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS ? emulator.set_key('2') : emulator.unset_key('2');
    glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS ? emulator.set_key('3') : emulator.unset_key('3');
    glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS ? emulator.set_key('4') : emulator.unset_key('4');
    glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ? emulator.set_key('q') : emulator.unset_key('q');
    glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ? emulator.set_key('w') : emulator.unset_key('w');
    glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ? emulator.set_key('e') : emulator.unset_key('e');
    glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS ? emulator.set_key('r') : emulator.unset_key('r');
    glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ? emulator.set_key('a') : emulator.unset_key('a');
    glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? emulator.set_key('s') : emulator.unset_key('s');
    glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ? emulator.set_key('d') : emulator.unset_key('d');
    glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS ? emulator.set_key('f') : emulator.unset_key('f');
    glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS ? emulator.set_key('z') : emulator.unset_key('z');
    glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS ? emulator.set_key('x') : emulator.unset_key('x');
    glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS ? emulator.set_key('c') : emulator.unset_key('c');
    glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS ? emulator.set_key('v') : emulator.unset_key('v');

    // Emulator cycling
    double curr_time = glfwGetTime() * 1000;
    if (!control_panel.pause()) {
      double elapsed_time = curr_time - last_time;

      delay_accumulator += elapsed_time;
      if (delay_accumulator >= 16) {
        emulator.update_timers();
        delay_accumulator = 0;
      }

      uint32_t cycle_count = elapsed_time / (1000.0f / control_panel.clock_rate());
      for (uint32_t i = 0; i < cycle_count; i++) {
        emulator.cycle();
      }

      last_time = curr_time;
    } else {
      last_time = curr_time;
    }

    emulator.get_screen_rgb(screen_buffer);
    update_screen_texture(screen_texture, screen_width, screen_height, screen_buffer);

    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    draw_screen_texture(screen_texture);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // App ImGui Componenets
    control_panel.render();
    if (control_panel.reload()) {
      auto rom_path = control_panel.get_selected_rom();
      auto rom_data = open_file(rom_path);

      auto start_addr = control_panel.start_addr();
      auto load_addr = control_panel.load_addr();

      emulator.set_program_counter(start_addr);
      emulator.load_rom(load_addr, rom_data, rom_data.size());

      control_panel.unset_reload();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

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
  PROFILE_FUNCTION();
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
  PROFILE_FUNCTION();
  glBindTexture(GL_TEXTURE_2D, texture);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f); glVertex2f(-1, 1);
  glTexCoord2f(1.0f, 0.0f); glVertex2f(1, 1);
  glTexCoord2f(1.0f, 1.0f); glVertex2f(1, -1);
  glTexCoord2f(0.0f, 1.0f); glVertex2f(-1, -1);
  glEnd();
}

