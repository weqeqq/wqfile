
#pragma once

#include <format>
#include <string_view>
#include <stdexcept>

namespace File {
class Error : public std::runtime_error {
protected:
  explicit Error(std::string_view) : std::runtime_error("") {}

  static std::string Namespace(
    std::string_view prefix, std::string_view message
  ) {
    return std::format("{}::{}", prefix, message);
  }
  static std::string Message(
    std::string_view prefix, std::string_view message
  ) {
    return std::format("{}: {}", prefix, message);
  }
};
};
