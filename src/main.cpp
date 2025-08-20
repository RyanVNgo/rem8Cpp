/*  @file   main.cpp
 *  @author Ryan V. Ngo
 */

#include "imgui.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

#include "emulator.h"
#include "user_interface/window.h"
#include "user_interface/graphics.h"
#include "widgets/widgets.h"
#include "widgets/control_panel.h"
#include "utilities/file.h"


int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  ApplicationWindow app_window{"rem8C++", 640, 320};
  if (!app_window.valid()) {
    std::cerr << "Failed to create application window" << std::endl;
    glfwTerminate();
    return -1;
  }

  app_window.make_current_context();
  app_window.set_vsync(true);

  WidgetRunner widget_runner{app_window.window()};
  ImGuiIO& io = ImGui::GetIO();

  auto emulator = rem8Cpp();
  auto control_panel = ControlPanel(emulator, io);

  widget_runner.add_widget(&control_panel);

  size_t screen_width = emulator.width();
  size_t screen_height = emulator.height();
  Screen screen{screen_width, screen_height};
  std::vector<unsigned char> screen_buffer(screen_width * screen_height * 3);

  // Main loop
  double last_time = 0;
  uint32_t delay_accumulator = 0;
  while (!app_window.should_close()) {
    glfwPollEvents();
    app_window.is_key_pressed(GLFW_KEY_1) ? emulator.set_key('1') : emulator.unset_key('1'); 
    app_window.is_key_pressed(GLFW_KEY_2) ? emulator.set_key('2') : emulator.unset_key('2');
    app_window.is_key_pressed(GLFW_KEY_3) ? emulator.set_key('3') : emulator.unset_key('3');
    app_window.is_key_pressed(GLFW_KEY_4) ? emulator.set_key('4') : emulator.unset_key('4');
    app_window.is_key_pressed(GLFW_KEY_Q) ? emulator.set_key('q') : emulator.unset_key('q');
    app_window.is_key_pressed(GLFW_KEY_W) ? emulator.set_key('w') : emulator.unset_key('w');
    app_window.is_key_pressed(GLFW_KEY_E) ? emulator.set_key('e') : emulator.unset_key('e');
    app_window.is_key_pressed(GLFW_KEY_R) ? emulator.set_key('r') : emulator.unset_key('r');
    app_window.is_key_pressed(GLFW_KEY_A) ? emulator.set_key('a') : emulator.unset_key('a');
    app_window.is_key_pressed(GLFW_KEY_S) ? emulator.set_key('s') : emulator.unset_key('s');
    app_window.is_key_pressed(GLFW_KEY_D) ? emulator.set_key('d') : emulator.unset_key('d');
    app_window.is_key_pressed(GLFW_KEY_F) ? emulator.set_key('f') : emulator.unset_key('f');
    app_window.is_key_pressed(GLFW_KEY_Z) ? emulator.set_key('z') : emulator.unset_key('z');
    app_window.is_key_pressed(GLFW_KEY_X) ? emulator.set_key('x') : emulator.unset_key('x');
    app_window.is_key_pressed(GLFW_KEY_C) ? emulator.set_key('c') : emulator.unset_key('c');
    app_window.is_key_pressed(GLFW_KEY_V) ? emulator.set_key('v') : emulator.unset_key('v');

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
    screen.update(0, 0, screen_width, screen_height, screen_buffer.data());

    std::size_t win_width{};
    std::size_t win_height{};
    app_window.frame_buff_size(win_width, win_height);
    update_viewport(win_width, win_height);

    clear();
    screen.draw();
    widget_runner.render();

    if (control_panel.reload()) {
      auto rom_path = control_panel.get_selected_rom();
      auto rom_data = open_file(rom_path);

      auto start_addr = control_panel.start_addr();
      auto load_addr = control_panel.load_addr();

      emulator.set_program_counter(start_addr);
      emulator.load_rom(load_addr, rom_data, rom_data.size());

      control_panel.unset_reload();
    }

    app_window.swap_buffers();
  }

  glfwTerminate();
  return 0;
}

