
#include "file/error.h"
#include <file/input.h>
#include <filesystem>

namespace File {
//
std::vector<std::uint8_t>
From(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    throw Error("File does not exist");
  }
  if (!std::filesystem::is_regular_file(path)) {
    throw Error("Path is not a regular file");
  }
  std::ifstream file(
    path,
    std::ios::binary | std::ios::ate
  );
  if (!file) {
    throw Error("Failed to open file: " + path.string());
  }
  auto length = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<std::uint8_t> buffer(length);
  if (!file.read(reinterpret_cast<char *>(buffer.data()), length)) {
    throw Error("Failed to read file: " + path.string());
  }
  return buffer;
}
std::string
StringFrom(const std::filesystem::path &path) {
  auto binary = From(path);
  return std::string(binary.begin(), binary.end());
}
} // namespace File
