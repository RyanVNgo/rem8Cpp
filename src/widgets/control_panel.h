/*  @file   control_panel.h
 *  @brief  Declaration of control panel
 *  @author Ryan V. Ngo
 */

#pragma once

#include "imgui.h"

#include "file_explorer.h"


class ControlPanel{
  public:
    ControlPanel(ImGuiIO& io);
    
    void render();

  private:
    FileExplorer file_explorer_;
    ImGuiIO& io_;

};


