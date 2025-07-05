
#include "file/error.h"
#include <file/output.h>

#include <fstream>

namespace File {
//
void To(
  const std::vector<std::uint8_t> &data,
  const std::filesystem::path &path
) {
  std::ofstream file(path, std::ios::binary);
  if (!file) {
    throw Error("Failed to open file for writing: " + path.string());
  }
  file.write(
    reinterpret_cast<const char *>(data.data()),
    data.size()
  );
  if (!file) {
    throw Error("Failed to write to file: " + path.string());
  }
}
void StringTo(
  const std::string &data,
  const std::filesystem::path &path
) {
  std::ofstream file(path);
  if (!file) {
    throw Error("Failed to open file for writing: " + path.string());
  }
  file << data;
  if (!file) {
    throw Error("Failed to write to file: " + path.string());
  }
}
} // namespace File
