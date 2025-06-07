
#pragma once 

#include <file/write.h>
#include <filesystem>

namespace File { namespace {

void WriteString(
  const std::filesystem::path &path,
  const std::string &content
) {
  Write(path, ByteSpan(
    reinterpret_cast<const Byte *>(content.data()),
    reinterpret_cast<const Byte *>(content.data()) + content.size()
  ));
}

}
}
