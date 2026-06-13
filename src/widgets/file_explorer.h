
#pragma once

#include <filesystem>


class FileExplorer {
    public:
        FileExplorer();

        auto open(std::filesystem::path init_path) -> void;

        auto get_selected_path() -> std::filesystem::path;

        [[nodiscard]] auto is_shown() const noexcept -> bool;

        auto render() noexcept -> void;

    private:
        bool shown_;
        std::filesystem::path selected_path_;
        std::filesystem::path temp_dir_;
        std::filesystem::path temp_path_;

};

