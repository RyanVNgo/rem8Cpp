/*  @file   file.cpp
 *  @brief  Definition of file utilities.
 *  @author Ryan V. Ngo
 */

#include "file.h"

#include <fstream>


std::vector<char> open_file(std::filesystem::path file_path) {
  std::vector<char> file_bytes(0);

  if (std::filesystem::exists(file_path)) {
    std::ifstream file(file_path, std::ios::binary);

    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    file_bytes.resize(file_size);
    file.read(file_bytes.data(), file_size);
  }

  return file_bytes;
}

