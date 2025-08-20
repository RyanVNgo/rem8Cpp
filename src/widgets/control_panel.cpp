/*  @file   control_panel.h
 *  @brief  Definition of control panel.
 *  @author Ryan V. Ngo
 */

#include "control_panel.h"

#include <chrono>

#include "imgui.h"


ControlPanel::ControlPanel(rem8Cpp& emulator) 
  : m_emulator(emulator),
    m_io(ImGui::GetIO()),
    file_explorer_(FileExplorer()),
    m_time_last(std::chrono::high_resolution_clock::now()),
    m_time_curr(std::chrono::high_resolution_clock::now()),
    m_framerate(0.0f),
    m_pause(true),
    m_load_addr(0x0200),
    m_start_addr(0x0200),
    m_clock_rate(1000),
    reload_(false)
{ }

void ControlPanel::render() {
  ImGui::Begin("Control Panel");

  m_time_curr = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(m_time_curr - m_time_last);
  if (time_span.count() >= 0.1f) {
    m_framerate = m_io.Framerate;
    m_time_last = m_time_curr;
  }

  ImGui::Text("fps: %.3f", m_framerate); 

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button(m_pause ? "PAUSED" : "PLAYING")) {
    m_pause = !m_pause;
  }

  ImGui::Text("ROM: %s\n", m_selected_rom.c_str());
  if (ImGui::Button("Open ROM")) {
    file_explorer_.open(std::getenv("HOME"));
  }

  ImGui::DragScalar("Load Addr", ImGuiDataType_U16, &m_load_addr, 1.0f, NULL, NULL, "0x%04X");
  ImGui::DragScalar("Start Addr", ImGuiDataType_U16, &m_start_addr, 1.0f, NULL, NULL, "0x%04X");
  ImGui::DragInt("Clock Rate", &m_clock_rate, 1.0f, 0, 20000, "%d HZ");

  if (file_explorer_.is_shown()) {
    file_explorer_.render();
    if (!file_explorer_.is_shown()) {
      m_selected_rom = file_explorer_.get_selected_path();
      reload_ = true;
      m_pause = true;
    }
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Text("DIAGNOSTICS"); 
  ImGui::Text("Program Counter:   0x%04hX", m_emulator.program_counter()); // Program Counter
  ImGui::Text("Address Register:  0x%04hX", m_emulator.I_register());      // Address Register
  ImGui::Text("Stack Pointer:     0x%04hX", m_emulator.stack_pointer());   // Stack Pointer 
  ImGui::Text("Delay Timer:       0x%02hhX", m_emulator.delay_timer());    // Stack Pointer 
  ImGui::Text("Sound Timer:       0x%02hhX", m_emulator.sound_timer());    // Stack Pointer 

  // Data Registers
  for (uint8_t i = 0; i < 0x10; i++) {
    ImGui::Text("V%hhX:0x%02hhX", i, m_emulator.data_register(i)); 
    if (i % 4 != 3) {
      ImGui::SameLine();
      ImGui::Text("|");
      ImGui::SameLine();
    }
  }

  // Memory from PC
  ImGui::Text("Memory");
  for (uint16_t i = 0; i <= 0x8; i++) {
    uint16_t addr = m_emulator.program_counter() + i;
    ImGui::Text("%02hhX ", m_emulator.read_memory(addr));
    ImGui::SameLine();
  }

  ImGui::End();
}

bool ControlPanel::pause() const {
  return m_pause;
}

uint16_t ControlPanel::load_addr() const {
  return m_load_addr;
}

uint16_t ControlPanel::start_addr() const {
  return m_start_addr;
}

int ControlPanel::clock_rate() const {
  return m_clock_rate;
}

bool ControlPanel::reload() const {
  return reload_;
}

std::filesystem::path ControlPanel::get_selected_rom() const {
  return m_selected_rom;
}

void ControlPanel::unset_reload() {
  reload_ = false;
}

