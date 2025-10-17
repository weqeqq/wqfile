
#include <wqfile/file.h>

#include <gtest/gtest.h>

// A test fixture for file operations, managing a temporary directory.
class FileTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Create a unique temporary directory for each test run.
    test_dir_ = std::filesystem::temp_directory_path() / "file_cpp_test_suite";
    std::filesystem::create_directory(test_dir_);
  }

  void TearDown() override {
    // Clean up the temporary directory after tests.
    std::filesystem::remove_all(test_dir_);
  }

  // Helper to get a full path for a test file.
  std::filesystem::path GetTestPath(const std::string &filename) const {
    return test_dir_ / filename;
  }

  std::filesystem::path test_dir_;
};

// --- Happy Path Tests ---

TEST_F(FileTest, WriteAndReadBinary) {
  const auto path = GetTestPath("test.bin");
  const std::vector<unsigned char> data = {0, 1, 2, 3, 255, 128, '\0', 42};

  ASSERT_NO_THROW(File::WriteBinary(path, data));
  ASSERT_TRUE(std::filesystem::exists(path));

  const auto read_data = File::ReadBinary(path);
  ASSERT_EQ(data, read_data);
}

TEST_F(FileTest, WriteAndReadString) {
  const auto path = GetTestPath("test.txt");
  const std::string data = "Hello, world!\nThis is a test file.";

  ASSERT_NO_THROW(File::WriteString(path, data));
  ASSERT_TRUE(std::filesystem::exists(path));

  const auto read_data = File::ReadString(path);
  ASSERT_EQ(data, read_data);
}

TEST_F(FileTest, WriteBinaryReadString) {
  const auto path = GetTestPath("binary_to_string.bin");
  const std::vector<unsigned char> data = {'H', 'e', 'l', 'l', 'o', '\0',
                                           'W', 'o', 'r', 'l', 'd'};

  File::WriteBinary(path, data);
  const auto read_string = File::ReadString(path);

  const std::string expected_string(data.begin(), data.end());
  ASSERT_EQ(read_string.size(), data.size());
  ASSERT_EQ(read_string, expected_string);
}

TEST_F(FileTest, WriteStringReadBinary) {
  const auto path = GetTestPath("string_to_binary.txt");
  const std::string data = "Some ASCII data";

  File::WriteString(path, data);
  const auto read_binary = File::ReadBinary(path);

  const std::vector<unsigned char> expected_binary(data.begin(), data.end());
  ASSERT_EQ(read_binary, expected_binary);
}

TEST_F(FileTest, ReadWriteEmptyFile) {
  const auto path = GetTestPath("empty.dat");

  // Test writing and reading an empty binary file
  const std::vector<unsigned char> empty_binary;
  ASSERT_NO_THROW(File::WriteBinary(path, empty_binary));
  ASSERT_TRUE(std::filesystem::exists(path));
  ASSERT_EQ(std::filesystem::file_size(path), 0);
  ASSERT_EQ(File::ReadBinary(path), empty_binary);

  // Test writing and reading an empty string file
  // NOTE: This part depends on the bug fix for `File::WriteString`.
  const std::string empty_string;
  ASSERT_NO_THROW(File::WriteString(path, empty_string));
  ASSERT_TRUE(std::filesystem::exists(path));
  ASSERT_EQ(std::filesystem::file_size(path), 0);
  ASSERT_EQ(File::ReadString(path), empty_string);
}

// --- Error Handling Tests ---

TEST_F(FileTest, ReadNonExistentFile) {
  const auto path = GetTestPath("does_not_exist.txt");
  ASSERT_THROW(File::ReadBinary(path), File::ReadError);
  ASSERT_THROW(File::ReadString(path), File::ReadError);
}

TEST_F(FileTest, ReadDirectory) {
  // Attempting to read a directory should fail.
  const auto path = test_dir_;
  ASSERT_THROW(File::ReadBinary(path), File::ReadError);
  ASSERT_THROW(File::ReadString(path), File::ReadError);
}

TEST_F(FileTest, WriteToInvalidPath) {
  // Attempting to write to a file in a non-existent directory should fail.
  const auto path = test_dir_ / "nonexistent_subdir" / "file.txt";

  const std::vector<unsigned char> binary_data = {1, 2, 3};
  ASSERT_THROW(File::WriteBinary(path, binary_data), File::WriteError);

  const std::string string_data = "test";
  ASSERT_THROW(File::WriteString(path, string_data), File::WriteError);
}
