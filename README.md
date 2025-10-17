# wqfile: Simple and Safe C++ File I/O

`wqfile` is a single-header C++ library that provides a set of convenient functions for simple and safe file I/O operations. It aims to simplify common file operations like reading and writing entire files, while ensuring robustness through an exception-based error handling mechanism. The API is designed to be clean and modern, using callable function objects for its core functionalities.

## Features

- Read entire files into binary buffers (`std::vector<unsigned char>`).
- Read entire files into strings (`std::string`).
- Write binary data to files from a `std::vector<unsigned char>`.
- Write string data to files from a `std::string`.
- A simple and clear exception hierarchy for error handling (`File::Error`, `File::ReadError`, `File::WriteError`).
- Modern C++ API using callable objects.
- Header-only for easy integration.

## Requirements

- A C++17 compliant compiler (e.g., GCC 7+, Clang 5+, MSVC v19.14+).

## Integration

`wqfile` is a header-only library. To use it, simply copy the `wqfile/` directory into your project's include path and include the main header:

```cpp
#include <wqfile/file.h>
```

## Usage

Here's a quick example demonstrating how to write to and read from a file using `wqfile`.

```cpp
#include <wqfile/file.h>
#include <iostream>
#include <vector>

int main() {
  try {
    // Write a string to a file
    File::WriteString("hello.txt", "Hello, World!");

    // Read the string back
    std::string content = File::ReadString("hello.txt");
    std::cout << "String content: " << content << std::endl;

    // Create some binary data
    std::vector<unsigned char> data = {0xDE, 0xAD, 0xBE, 0xEF};
    File::WriteBinary("data.bin", data);

    // Read binary data back
    std::vector<unsigned char> read_data = File::ReadBinary("data.bin");
    std::cout << "Read " << read_data.size() << " bytes of binary data." << std::endl;

  } catch (const File::Error& e) {
    std::cerr << "File operation failed: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
```

## API Reference

The library provides four main functions within the `File` namespace. All functions take a `std::filesystem::path` as the file path argument.

### Reading Files

- **`File::ReadString(path)`**
  Reads the entire content of a file into a `std::string`. Throws `File::ReadError` on failure.

- **`File::ReadBinary(path)`**
  Reads the entire content of a file into a `std::vector<unsigned char>`. Throws `File::ReadError` on failure.

### Writing Files

- **`File::WriteString(path, data)`**
  Writes a `std::string` to a file. The file is created if it doesn't exist and overwritten if it does. Throws `File::WriteError` on failure.

- **`File::WriteBinary(path, data)`**
  Writes a `std::vector<unsigned char>` to a file. The file is created if it doesn't exist and overwritten if it does. Throws `File::WriteError` on failure.

## Error Handling

All file operations can throw exceptions on failure. The library provides a simple exception hierarchy for granular error handling.

- `File::Error`: The base exception class for all errors from this library. It inherits from `std::runtime_error`.
- `File::ReadError`: Thrown specifically for errors during read operations (e.g., file not found, not a regular file, read permission denied).
- `File::WriteError`: Thrown specifically for errors during write operations (e.g., cannot open file for writing, disk full).

You can catch the base `File::Error` to handle any exception from the library:

```cpp
try {
  std::string content = File::ReadString("non_existent_file.txt");
} catch (const File::Error& e) {
  // This will catch ReadError and WriteError as well
  std::cerr << "File operation failed: " << e.what() << std::endl;
}
```
