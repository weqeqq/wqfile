cmake_minimum_required(VERSION 3.23)

project(
  file 
  LANGUAGES CXX VERSION 0.1.0
)
add_library(file INTERFACE)
add_library(file::file ALIAS file)

option(BUILD_TEST "Build tests" OFF)

target_compile_features(file INTERFACE cxx_std_23)
target_sources(file PUBLIC 
  FILE_SET  HEADERS 
  BASE_DIRS include
  FILES 
  include/file/stream.h
  include/file/read_string.h
  include/file/read.h 
  include/file/write_string.h
  include/file/write.h
  include/file/error.h
  include/file/type.h
)
if(BUILD_TEST)
add_subdirectory(test)
endif()

install(TARGETS file FILE_SET HEADERS)
