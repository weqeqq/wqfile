
#pragma once

#include <file/type.h>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <cerrno>
#include <file/error.h>
#include <string>

namespace File {

class ReadError : public Error {
protected:
  explicit ReadError(
    const std::string &message

  ) : Error(Namespace("ReadError", message)) {}
};
namespace ReadErrorMessage {
struct OpenFailure : ReadError {
  explicit OpenFailure(
    const std::string &message

  ) : ReadError(Message("OpenFailure", message)) {}
};
struct ReadFailure : ReadError {
  explicit ReadFailure(
    const std::string &message 

  ) : ReadError(Message("ReadFailure", message)) {}
};
};
namespace Detail {

class ReadFunction {
public:
  ByteSequence operator()(const std::string &filename) const {
    std::ifstream stream = OpenStream(filename);
    std::uint64_t length = stream.tellg();

    stream.seekg(0, std::ios::beg); return Read(stream, length);
  }
private:
  std::ifstream OpenStream(const std::string &filename) const {
    std::ifstream stream(
      filename,
      std::ios::binary | std::ios::ate
    );
    if (!stream) {
      throw ReadErrorMessage::OpenFailure(std::strerror(errno));
    }
    return stream;
  }
  ByteSequence Read(
    std::ifstream &stream, std::uint64_t length
  ) const {
    ByteSequence data(length);
    stream.read(
      reinterpret_cast<char *>(data.data()),
      length
    );
    if (!stream) {
      throw ReadErrorMessage::ReadFailure(std::strerror(errno));
    }
    return data;
  }
};
};
inline constexpr auto Read = Detail::ReadFunction();
};
