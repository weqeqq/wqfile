/**
 * @mainpage file-cpp: Simple and Safe C++ File I/O
 *
 * @section intro_sec Introduction
 *
 * `file-cpp` is a single-header C++ library that provides a set of convenient
 * functions for simple and safe file I/O operations. It aims to simplify
 * common file operations like reading and writing entire files, while ensuring
 * robustness through an exception-based error handling mechanism. The API is
 * designed to be clean and modern, using callable function objects for its
 * core functionalities.
 *
 * @section features_sec Features
 *
 * - Read entire files into binary buffers (`std::vector<unsigned char>`).
 * - Read entire files into strings (`std::string`).
 * - Write binary data to files from a `std::vector<unsigned char>`.
 * - Write string data to files from a `std::string`.
 * - A simple and clear exception hierarchy for error handling (`File::Error`,
 * `File::ReadError`, `File::WriteError`).
 * - Modern C++ API using callable objects (`constexpr` function objects).
 * - Header-only for easy integration.
 *
 * @section usage_sec Example Usage
 *
 * Here's a quick example demonstrating how to write to and read from a file
 * using `file-cpp`.
 *
 * @code
 * #include <file-cpp/file.h>
 * #include <iostream>
 * #include <vector>
 *
 * int main() {
 *   try {
 *     // Write a string to a file
 *     File::WriteString("hello.txt", "Hello, World!");
 *
 *     // Read the string back
 *     std::string content = File::ReadString("hello.txt");
 *     std::cout << "String content: " << content << std::endl;
 *
 *     // Create some binary data
 *     std::vector<unsigned char> data = {0xDE, 0xAD, 0xBE, 0xEF};
 *     File::WriteBinary("data.bin", data);
 *
 *     // Read binary data back
 *     std::vector<unsigned char> read_data = File::ReadBinary("data.bin");
 *     std::cout << "Read " << read_data.size() << " bytes of binary data." <<
 * std::endl;
 *
 *   } catch (const File::Error& e) {
 *     std::cerr << "File operation failed: " << e.what() << std::endl;
 *     return 1;
 *   }
 *   return 0;
 * }
 * @endcode
 *
 * For more details on the available functions, see the `File` namespace
 * documentation.
 */

/**
 * @file file.h
 * @brief Defines utility functions for simple and safe file I/O operations.
 *
 * This header provides the `File` namespace, which contains a set of
 * convenient functions for reading from and writing to files. It aims to
 * simplify common file operations while ensuring robustness through
 * exception-based error handling.
 *
 * Features:
 * - Reading files into binary buffers (`std::vector<unsigned char>`).
 * - Reading files into strings (`std::string`).
 * - Writing binary data to files.
 * - Writing string data to files.
 * - A simple exception hierarchy (`File::Error`, `File::ReadError`,
 * `File::WriteError`).
 *
 * The functions are implemented as callable objects (`constexpr` function
 * objects) for a clean and modern C++ API.
 *
 * Example Usage:
 * @code
 * #include <file-cpp/file.h>
 * #include <iostream>
 *
 * int main() {
 *   try {
 *     // Write a string to a file
 *     File::WriteString("hello.txt", "Hello, World!");
 *
 *     // Read the string back
 *     std::string content = File::ReadString("hello.txt");
 *     std::cout << content << std::endl;
 *
 *     // Create some binary data
 *     std::vector<unsigned char> data = {0xDE, 0xAD, 0xBE, 0xEF};
 *     File::WriteBinary("data.bin", data);
 *
 *     // Read binary data back
 *     std::vector<unsigned char> read_data = File::ReadBinary("data.bin");
 *
 *   } catch (const File::Error& e) {
 *     std::cerr << "File operation failed: " << e.what() << std::endl;
 *     return 1;
 *   }
 *   return 0;
 * }
 * @endcode
 */
#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace File {
/**
 * @brief Base class for all file-related exceptions.
 *
 * This exception can be caught to handle any error from the File namespace.
 */
struct Error : std::runtime_error {
  /**
   * @brief Constructs an Error exception.
   * @param message The error message.
   */
  Error(std::string_view message) : std::runtime_error(message.data()) {}
};
/**
 * @brief Exception thrown on file read errors.
 *
 * This exception is a subclass of File::Error and is thrown specifically
 * when a read operation fails.
 */
struct ReadError : Error {
  using Error::Error;
};
/**
 * @brief Exception thrown on file write errors.
 *
 * This exception is a subclass of File::Error and is thrown specifically
 * when a write operation fails.
 */
struct WriteError : Error {
  using Error::Error;
};
namespace {
struct ReadBinaryFn {
  std::vector<unsigned char>
  operator()(const std::filesystem::path &path) const {
    if (!std::filesystem::exists(path)) {
      throw ReadError(std::format("File ({}) does not exists", path.string()));
    }
    if (!std::filesystem::is_regular_file(path)) {
      throw ReadError(
          std::format("Path ({}) is not a regular file", path.string()));
    }
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
      throw ReadError(std::format("Failed to open file ({})", path.string()));
    }
    auto length = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(length);
    if (!file.read(reinterpret_cast<char *>(buffer.data()), length)) {
      throw ReadError(std::format("Failed to read file ({})", path.string()));
    }
    return buffer;
  }
};
} // namespace
/**
 * @brief Reads the entire content of a file into a binary buffer.
 *
 * This callable object reads all bytes from the file at the specified path and
 * returns them in a `std::vector<unsigned char>`.
 *
 * @param path The path to the file to read.
 * @return A `std::vector<unsigned char>` containing the binary data of the
 * file.
 * @throws ReadError if the file does not exist, is not a regular file, or if
 *                   an I/O error occurs during the read operation.
 */
inline constexpr auto ReadBinary = ReadBinaryFn();

namespace {
struct ReadStringFn {
  std::string operator()(const std::filesystem::path &path) const {
    auto binary = ReadBinary(path);
    return std::string(binary.begin(), binary.end());
  }
};
} // namespace
/**
 * @brief Reads the entire content of a file into a string.
 *
 * This callable object reads all bytes from the file at the specified path and
 * returns them as a `std::string`. This function is a convenience wrapper
 * around `ReadBinary`.
 *
 * @param path The path to the file to read.
 * @return A `std::string` containing the content of the file.
 * @throws ReadError if the file does not exist, is not a regular file, or if
 *                   an I/O error occurs during the read operation.
 */
inline constexpr auto ReadString = ReadStringFn();

namespace {
struct WriteBinaryFn {
  void operator()(const std::filesystem::path &path,
                  const std::vector<unsigned char> &data) const {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
      throw WriteError(
          std::format("Failed to open file for writing ({})", path.string()));
    }
    file.write(reinterpret_cast<const char *>(data.data()), data.size());
    if (!file) {
      throw WriteError(
          std::format("Failed to write to file ({})", path.string()));
    }
  }
};
} // namespace
/**
 * @brief Writes a binary buffer to a file.
 *
 * This callable object writes the content of a `std::vector<unsigned char>`
 * to the file at the specified path. If the file already exists, it is
 * overwritten.
 *
 * @param path The path to the file to write.
 * @param data The `std::vector<unsigned char>` containing the binary data to
 * write.
 * @throws WriteError if the file cannot be opened for writing or if an I/O
 *                    error occurs during the write operation.
 */
inline constexpr auto WriteBinary = WriteBinaryFn();

namespace {
struct WriteStringFn {
  void operator()(const std::filesystem::path &path,
                  const std::string &data) const {
    std::ofstream file(path);
    if (!file) {
      throw WriteError(
          std::format("Failed to open file for writing ({})", path.string()));
    }
    file << data;
    if (!file) {
      throw WriteError(
          std::format("Failed to write to file ({})", path.string()));
    }
  }
};
} // namespace
/**
 * @brief Writes a string to a file.
 *
 * This callable object writes the content of a `std::string` to the file at
 * the specified path. If the file already exists, it is overwritten.
 *
 * @param path The path to the file to write.
 * @param data The `std::string` containing the data to write.
 * @throws WriteError if the file cannot be opened for writing or if an I/O
 *                    error occurs during the write operation.
 */
inline constexpr auto WriteString = WriteStringFn();

}; // namespace File
