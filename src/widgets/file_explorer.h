/*  @file   file_explorer.h
 *  @brief  Declaration of file explorer.
 *  @author Ryan V. Ngo
 */

#pragma once

#include <filesystem>


class FileExplorer {
  public:
    FileExplorer();
    
    void show(std::filesystem::path init_path);
    void render();

    const std::filesystem::path& get_selected_path() const;

  private:
    bool shown_;
    std::filesystem::path selected_path_;
    std::filesystem::path temp_dir_;
    std::filesystem::path temp_path_;

};

