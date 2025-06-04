/*  @file   control_panel.h
 *  @brief  Declaration of control panel
 *  @author Ryan V. Ngo
 */

#pragma once

#include <filesystem>
#include <chrono>

#include "imgui.h"

#include "file_explorer.h"


class ControlPanel{
  public:
    ControlPanel(ImGuiIO& io);
    
    void render();
    bool pause() const;
    uint16_t load_addr() const;
    uint16_t start_addr() const;
    bool reload() const;
    std::filesystem::path get_selected_rom() const;
    void unset_reload();

  private:
    FileExplorer file_explorer_;
    std::chrono::high_resolution_clock::time_point time_last_;
    std::chrono::high_resolution_clock::time_point time_curr_;
    float framerate_;
    ImGuiIO& io_;
    bool pause_;
    std::filesystem::path selected_rom_;
    uint16_t load_addr_;
    uint16_t start_addr_;

    bool reload_;

};


