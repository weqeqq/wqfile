
add_executable(tests)
target_sources(tests
  PRIVATE
    sources/input_test.cc
    sources/output_test.cc
)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
target_link_libraries(tests
    PRIVATE
        file::file
        GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(tests)
