
#pragma once

#include <stdexcept>

namespace File {

struct Error : std::runtime_error {
  explicit Error(const std::string &message) : std::runtime_error(message) {}
};
};
