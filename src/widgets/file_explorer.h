/*  @file   file_explorer.h
 *  @brief  Declaration of file explorer.
 *  @author Ryan V. Ngo
 */

#pragma once

#include <filesystem>


class FileExplorer {
  public:
    FileExplorer();
    
    void open(std::filesystem::path init_path);
    std::filesystem::path get_selected_path();
    bool is_shown();
    void render();

  private:
    bool shown_;
    std::filesystem::path selected_path_;
    std::filesystem::path temp_dir_;
    std::filesystem::path temp_path_;

};

