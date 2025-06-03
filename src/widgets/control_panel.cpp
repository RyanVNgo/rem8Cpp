/*  @file   control_panel.h
 *  @brief  Definition of control panel.
 *  @author Ryan V. Ngo
 */

#include "control_panel.h"

#include "imgui.h"


ControlPanel::ControlPanel(ImGuiIO& io) 
  : file_explorer_(FileExplorer()),
    io_(io)
{ }

void ControlPanel::render() {
  ImGui::Begin("Control Panel");

  ImGui::Text("mpf: %.3f", 1000.0f / io_.Framerate);
  ImGui::Text("fps: %.3f", io_.Framerate);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Text("ROM: %s\n", file_explorer_.get_selected_path().c_str());
  if (ImGui::Button("Load ROM")) {
    file_explorer_.show(std::getenv("HOME"));
  }

  file_explorer_.render();
  ImGui::End();
}


