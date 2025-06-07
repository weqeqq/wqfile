
include(cmake/target/gtest.cmake)

add_executable(file-test)

target_link_libraries(file-test 
  target::gtest file::file
)
target_sources(file-test
  PRIVATE
    source/read_string_test.cc
    source/read_test.cc
    source/stream_test.cc
    source/write_string_test.cc
    source/write_test.cc
)
enable_testing()
include(GoogleTest)

gtest_discover_tests(file-test)
