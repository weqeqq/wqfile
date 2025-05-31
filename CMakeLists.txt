cmake_minimum_required(VERSION 3.23)

project(file LANGUAGES CXX VERSION 0.1.0)

add_library(file)
add_library(file::file ALIAS file)

set_target_properties(file PROPERTIES
  CXX_STANDARD          23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS        OFF
)
target_sources(file PUBLIC 
  FILE_SET  HEADERS 
  BASE_DIRS include
  FILES 
  include/file/stream.h
  include/file/read.h 
  include/file/write.h
  include/file/error.h
  include/file/type.h
)
include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
  exception-project
  GIT_REPOSITORY https://github.com/weqeqq/exception-cpp.git
  GIT_TAG        42843f32621a206b999bac52d25f6ca0d821f440
  GIT_PROGRESS   TRUE
)
FetchContent_Declare(
  iterable-project
  GIT_REPOSITORY https://github.com/weqeqq/iterable-cpp.git
  GIT_TAG        359d3547cc1fada5f82b8a0f2c7b8ef6be671efe
  GIT_PROGRESS   TRUE
)
FetchContent_Declare(
  unicode-project 
  GIT_REPOSITORY https://github.com/weqeqq/unicode-cpp.git
  GIT_TAG        26a83df7e10fdd9af77951e225c03abf237109dd
  GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(
  exception-project
  iterable-project
  unicode-project
)

target_link_libraries(file PUBLIC
  exception::exception
  iterable::iterable
  unicode::unicode
)
install(
  TARGETS file 
  EXPORT  file-targets FILE_SET HEADERS
)
install(
  EXPORT    file-targets 
  NAMESPACE file:: DESTINATION lib/cmake/file
)
include(CMakePackageConfigHelpers)

set(FILE_INCLUDE_DIR include)
set(FILE_LIBRARY_DIR lib)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/file-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/file-config.cmake
  INSTALL_DESTINATION lib/cmake/file 
  PATH_VARS 
    FILE_INCLUDE_DIR 
    FILE_LIBRARY_DIR
)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/file-config-version.cmake COMPATIBILITY SameMajorVersion
)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/file-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/file-config-version.cmake
  DESTINATION lib/cmake/file
)
