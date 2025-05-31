
#pragma once

#include <file/stream.h>
#include <file/error.h>
#include <fstream>
#include <file/type.h>

namespace File {

class WriteError : public Error {
public:
  explicit WriteError(
    const std::string &message

  ) : Error(Namespace("WriteError", message)) {}
};
namespace WriteErrorMessage {
struct OpenFailure : public WriteError {
  explicit OpenFailure(
    const std::string &message 
  ) : WriteError(Message("OpenFailure", message)) {}
};
struct WriteFailure : public WriteError {
  explicit WriteFailure(
    const std::string &message

  ) : WriteError(Message("WriteFailure", message)) {}
};
};
namespace Detail {
class WriteFunction {
public:
  void operator()(
    const std::string &filename, const ByteSpan &data
  ) const {
    auto stream = OpenStream(filename);

    Write(stream, data);
  }
  template <Endian UsedEndian>
  void operator()(
    const std::string &filename, Stream<UsedEndian> &stream
  ) const {
    stream.Rewind();

    operator()(filename, stream.template Read<ByteSpan>(
      stream.Length()
    ));
  }
private:
  std::ofstream OpenStream(const std::string &filename) const {
    std::ofstream stream(
      filename,
      std::ios::binary | std::ios::trunc
    );
    if (!stream) {
      throw WriteErrorMessage::OpenFailure(std::strerror(errno));
    }
    return stream;
  }
  void Write(
    std::ofstream &stream, const ByteSpan &data
  ) const {
    stream.write(
      reinterpret_cast<const char *>(data.data()),
      data.size()
    );
    if (!stream) {
      throw WriteErrorMessage::WriteFailure(std::strerror(errno));
    }
  }
};
};
inline constexpr auto Write = Detail::WriteFunction();
};
