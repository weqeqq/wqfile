
include(FetchContent)

set(FETCHCONTENT_QUIET 0)
FetchContent_Declare(
  gtest-project
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(gtest-project)

add_library(target::gtest ALIAS gtest_main)

