
#pragma once

#include <cerrno>
#include <cstring>
#include <file/error.h>
#include <memory>
#include <filesystem>
#include <vector>
#include <fstream>

#include <file/export.h>

namespace File {
//

FILE_EXPORT std::vector<std::uint8_t>
From(const std::filesystem::path &path);

FILE_EXPORT std::string
StringFrom(const std::filesystem::path &path);

}; // namespace File
