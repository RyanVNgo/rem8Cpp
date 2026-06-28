
#pragma once

#include <vector>
#include <filesystem>


auto open_file(const std::filesystem::path file_path) -> std::vector<char>;

