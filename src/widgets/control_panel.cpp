/*  @file   control_panel.h
 *  @brief  Definition of control panel.
 *  @author Ryan V. Ngo
 */

#include "control_panel.h"

#include "imgui.h"


ControlPanel::ControlPanel(ImGuiIO& io) 
  : file_explorer_(FileExplorer()),
    io_(io),
    pause_(true),
    load_addr_(0x0200),
    start_addr_(0x0200),
    reload_(false)
{ }

void ControlPanel::render() {
  ImGui::Begin("Control Panel");

  ImGui::Text("mpf: %.3f", 1000.0f / io_.Framerate);
  ImGui::Text("fps: %.3f", io_.Framerate);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button(pause_ ? "Paused" : "Resume")) {
    pause_ = !pause_;
  }

  ImGui::Text("ROM: %s\n", selected_rom_.c_str());
  if (ImGui::Button("Open ROM")) {
    file_explorer_.open(std::getenv("HOME"));
  }

  ImGui::DragScalar("Load Addr", ImGuiDataType_U16, &load_addr_, 1.0f, NULL, NULL, "0x%04X");
  ImGui::DragScalar("Start Addr", ImGuiDataType_U16, &start_addr_, 1.0f, NULL, NULL, "0x%04X");

  if (file_explorer_.is_shown()) {
    file_explorer_.render();
    if (!file_explorer_.is_shown()) {
      selected_rom_ = file_explorer_.get_selected_path();
      reload_ = true;
      pause_ = true;
    }
  }

  //ImGui::ShowDemoWindow();

  ImGui::End();
}

bool ControlPanel::pause() const {
  return pause_;
}

uint16_t ControlPanel::load_addr() const {
  return load_addr_;
}

uint16_t ControlPanel::start_addr() const {
  return start_addr_;
}

bool ControlPanel::reload() const {
  return reload_;
}

std::filesystem::path ControlPanel::get_selected_rom() const {
  return selected_rom_;
}

void ControlPanel::unset_reload() {
  reload_ = false;
}

