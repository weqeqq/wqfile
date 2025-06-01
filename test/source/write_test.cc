
#include "file/type.h"
#include <gtest/gtest.h>
#include <file/stream.h>
#include <file/write.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

class WriteTest : public ::testing::Test {
protected:
  void SetUp() override {
    temp_file_ = std::filesystem::temp_directory_path() / "test_file.bin";
  }

  void TearDown() override {
    std::filesystem::remove(temp_file_);
  }

  std::vector<uint8_t> ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      return {};
    }
    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
  }
  std::filesystem::path temp_file_;
};
TEST_F(WriteTest, WriteByteSpanSuccess) {
  auto data = File::ByteSequence{0x01, 0x02, 0x03, 0x04};

  EXPECT_NO_THROW(File::Write(temp_file_.string(), data));

  EXPECT_EQ(ReadFile(temp_file_.string()), data);
}

TEST_F(WriteTest, WriteStreamSuccess) {
  auto data = File::ByteSequence{0x05, 0x06, 0x07, 0x08};
  File::Stream stream(data);

  EXPECT_NO_THROW(File::Write(temp_file_.string(), stream));

  EXPECT_EQ(ReadFile(temp_file_.string()), data);
}

TEST_F(WriteTest, WriteFileOpenFailure) {
  auto data = File::ByteSequence{0x01, 0x02};
  std::string invalid_path = "/invalid_directory/test_file.bin"; // Assumes /invalid_directory doesn't exist

  EXPECT_THROW(File::Write(invalid_path, data), File::WriteErrorMessage::OpenFailure);
}
