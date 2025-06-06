/*  @file   main.cpp
 *  @brief  Program entry point.
 *  @author DearImGui & Ryan V. Ngo
 */

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#include <vector>

#include "emulator.h"
#include "widgets/control_panel.h"
#include "utilities/file.h"
#include "utilities/instrumentor.h"

void initialize_screen_texture(GLuint& texture, size_t width, size_t height);
void update_screen_texture(GLuint texture, size_t width, size_t height, std::vector<unsigned char>& data);
void draw_screen_texture(GLuint texture);

int main() {
#if defined(PROFILING)
  Instrumentor::Get().BeginSession("Main");
#endif

  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
      printf("Error: %s\n", SDL_GetError());
      return -1;
  }

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100 (WebGL 1.0)
  const char* glsl_version = "#version 100";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
  // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
  const char* glsl_version = "#version 300 es";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
  // GL 3.2 Core + GLSL 150
  const char* glsl_version = "#version 150";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

  // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

  constexpr size_t width = 640;
  constexpr size_t height = 320;
  SDL_Window* window = SDL_CreateWindow("rem8C++", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
  if (window == nullptr) {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return -1;
  }

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
    return -1;
  }

  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); 

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

  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  auto emulator = rem8Cpp();
  auto control_panel = ControlPanel(emulator, io);

  size_t screen_width = emulator.width();
  size_t screen_height = emulator.height();
  GLuint screen_texture;
  std::vector<unsigned char> screen_buffer(screen_width * screen_height * 3);
  initialize_screen_texture(screen_texture, screen_width, screen_height);

  // Main loop
  bool done = false;
  uint32_t last_time = 0;
  uint32_t delay_accumulator = 0;
  while (!done) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
          done = true;
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
          done = true;
      if (event.type == SDL_KEYDOWN) emulator.set_key(event.key.keysym.sym);
      if (event.type == SDL_KEYUP) emulator.unset_key(event.key.keysym.sym);
    }
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    // Emulator cycling
    { PROFILE_SCOPE("Emulator Cycling");
    uint32_t curr_time = SDL_GetTicks();
    if (!control_panel.pause()) {
      uint32_t elapsed_time = curr_time - last_time;

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
    } // End Profiling Scope

    { PROFILE_SCOPE("Update Screen");
    emulator.get_screen_rgb(screen_buffer);
    update_screen_texture(screen_texture, screen_width, screen_height, screen_buffer);
    } // End Profiling Scope

    { PROFILE_SCOPE("Render Screen");
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    draw_screen_texture(screen_texture);
    } // End Profiling Scope

    { PROFILE_SCOPE("Render ImGui");
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
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
    } // End Profiling Scope

    { PROFILE_SCOPE("Window Swapping");
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
      SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
    SDL_GL_SwapWindow(window);
    } // End Profiling Scope
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

#if defined(PROFILING)
  Instrumentor::Get().EndSession();
#endif
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

