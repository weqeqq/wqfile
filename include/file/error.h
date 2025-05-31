
#pragma once

#include <exception/error.h>

namespace File {
class Error : public Exception::Error {
protected:
  explicit Error(
    const std::string &message

  ) : Exception::Error(Namespace("File", message)) {}
};
};
