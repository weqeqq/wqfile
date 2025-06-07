
#pragma once 

#include <file/read.h>
#include <filesystem>

namespace File { namespace {

std::string ReadString(const std::filesystem::path &path) {
  return std::string(
    reinterpret_cast<const char *>(Read(path).data())
  );
}
}
}
