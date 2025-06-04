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
  temp_dir_ = init_path;
  shown_ = true;
}
std::filesystem::path FileExplorer::get_selected_path() {
  return selected_path_;
}

bool FileExplorer::is_shown() {
  return shown_;
}

void FileExplorer::render() {
  if (!shown_) return;
  ImGui::Begin("File Explorer", &shown_);

  if (ImGui::ArrowButton("Back", ImGuiDir_Up)) { temp_dir_ = temp_dir_.parent_path(); }
  ImGui::SameLine();
  ImGui::Text("%s", temp_path_.c_str());
  ImGui::SameLine();
  if (ImGui::Button("Open")) { 
    selected_path_ = temp_path_;
    shown_ = false;
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  std::vector<std::filesystem::path> dir_paths;
  std::vector<std::filesystem::path> file_paths;
  for (const auto& path : std::filesystem::directory_iterator(temp_dir_)) {
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
        temp_dir_ = path;
      }
    }
    
    for (const auto& path : file_paths) {
      ImGui::TableNextColumn();
      std::string label = path.filename().c_str();
      if(ImGui::Selectable(label.c_str())) {
        temp_path_ = path;
      }
    }

    ImGui::EndTable();
  }

  ImGui::End();
}

