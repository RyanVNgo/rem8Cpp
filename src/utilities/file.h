/*  @file   file.h
 *  @brief  Declaration of file utilities.
 *  @author Ryan V. Ngo
 */

#pragma once

#include <vector>
#include <filesystem>


std::vector<char> open_file(std::filesystem::path file_path);


