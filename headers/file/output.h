
#include <filesystem>
#include <vector>

#include <file/export.h>

namespace File {
//
FILE_EXPORT void To(
  const std::vector<std::uint8_t> &data,
  const std::filesystem::path &path
);
FILE_EXPORT void StringTo(
  const std::string &data,
  const std::filesystem::path &path
);

} // namespace File
