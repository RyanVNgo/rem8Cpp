/*  @file   file_explorer.cpp
 *  @brief  Implementation of file explorer.
 *  @author Ryan V. Ngo
 */

#include "file_explorer.h"

#include <filesystem>
#include <vector>
#include <algorithm>

#include "imgui.h"


FileExplorer::FileExplorer() 
  : shown_(false)
{ }

void FileExplorer::open(std::filesystem::path init_path) {
  m_temp_dir = init_path;
  shown_ = true;
}
std::filesystem::path FileExplorer::get_selected_path() {
  return m_selected_path;
}

bool FileExplorer::is_shown() {
  return shown_;
}

void FileExplorer::render() {
  if (!shown_) return;
  ImGui::Begin("File Explorer", &shown_);

  if (ImGui::ArrowButton("Back", ImGuiDir_Up)) { m_temp_dir = m_temp_dir.parent_path(); }
  ImGui::SameLine();
  ImGui::Text("%s", m_temp_path.c_str());
  ImGui::SameLine();
  if (ImGui::Button("Open")) { 
    m_selected_path = m_temp_path;
    shown_ = false;
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  std::vector<std::filesystem::path> dir_paths;
  std::vector<std::filesystem::path> file_paths;
  for (const auto& path : std::filesystem::directory_iterator(m_temp_dir)) {
      if (path.path().filename().c_str()[0] == '.') { continue; }
      if (!std::filesystem::is_directory(path)) { 
        file_paths.push_back(path);
        continue;
      }
      dir_paths.push_back(path);
  }
  std::sort(dir_paths.begin(), dir_paths.end());
  std::sort(file_paths.begin(), file_paths.end());

  if (ImGui::BeginTable("Files", 1, ImGuiTableFlags_RowBg)) {
    for (const auto& path : dir_paths) {
      ImGui::TableNextColumn();
      std::string label = path.filename().c_str();
      if (ImGui::Selectable(label.append("/").c_str())) {
        m_temp_dir = path;
      }
    }
    
    for (const auto& path : file_paths) {
      ImGui::TableNextColumn();
      std::string label = path.filename().c_str();
      if(ImGui::Selectable(label.c_str())) {
        m_temp_path = path;
      }
    }

    ImGui::EndTable();
  }

  ImGui::End();
}

