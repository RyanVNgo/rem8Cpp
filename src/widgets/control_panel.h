/*  @file   control_panel.h
 *  @brief  Declaration of control panel
 *  @author Ryan V. Ngo
 */

#pragma once

#include <filesystem>
#include <chrono>

#include "imgui.h"

#include "widgets.h"
#include "emulator.h"
#include "file_explorer.h"


class ControlPanel : public IWidget {
  public:
    ControlPanel(rem8Cpp& emulator);
    
    void render() override;
    bool pause() const;
    uint16_t load_addr() const;
    uint16_t start_addr() const;
    int clock_rate() const;
    bool reload() const;
    std::filesystem::path get_selected_rom() const;
    void unset_reload();

  private:
    rem8Cpp& m_emulator;
    ImGuiIO& m_io;
    FileExplorer file_explorer_;
    std::chrono::high_resolution_clock::time_point m_time_last;
    std::chrono::high_resolution_clock::time_point m_time_curr;
    float m_framerate;
    bool m_pause;
    std::filesystem::path m_selected_rom;
    uint16_t m_load_addr;
    uint16_t m_start_addr;
    int m_clock_rate;

    bool reload_;

};


