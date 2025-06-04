/*  @file   control_panel.h
 *  @brief  Definition of control panel.
 *  @author Ryan V. Ngo
 */

#include "control_panel.h"

#include <chrono>

#include "imgui.h"


ControlPanel::ControlPanel(ImGuiIO& io) 
  : file_explorer_(FileExplorer()),
    time_last_(std::chrono::high_resolution_clock::now()),
    time_curr_(std::chrono::high_resolution_clock::now()),
    framerate_(0.0f),
    io_(io),
    pause_(true),
    load_addr_(0x0200),
    start_addr_(0x0200),
    reload_(false)
{ }

void ControlPanel::render() {
  ImGui::Begin("Control Panel");

  time_curr_ = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(time_curr_ - time_last_);
  if (time_span.count() >= 0.1f) {
    framerate_ = io_.Framerate;
    time_last_ = time_curr_;
  }

  ImGui::Text("mpf: %.3f", 1000.0f / framerate_);
  ImGui::Text("fps: %.3f", framerate_); 

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button(pause_ ? "PAUSED" : "PLAYING")) {
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

