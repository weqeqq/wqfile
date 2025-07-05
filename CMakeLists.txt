cmake_minimum_required(VERSION 3.23)

project(file CXX)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
add_library(file)
include(GenerateExportHeader)
generate_export_header(
    file
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/headers/file/export.h
)
target_compile_features(file PUBLIC cxx_std_17)
target_sources(file PUBLIC
    FILE_SET HEADERS BASE_DIRS
        headers
        ${CMAKE_CURRENT_BINARY_DIR}/headers
    FILES
        headers/file/error.h
        headers/file/input.h
        headers/file/output.h
        ${CMAKE_CURRENT_BINARY_DIR}/headers/file/export.h
    PRIVATE
        sources/input.cc
        sources/output.cc
)
add_library(file::file ALIAS file)
option(FILE_BUILD_TESTS "" OFF)
if (FILE_BUILD_TESTS)
    message("Building tests for file")
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS file
    EXPORT file-targets
    FILE_SET HEADERS
)
install(EXPORT file-targets
    FILE file-targets.cmake
    NAMESPACE file::
    DESTINATION lib/cmake/file
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/file-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/file-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/file-config.cmake
    INSTALL_DESTINATION lib/cmake/file
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/file-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/file-config-version.cmake
    DESTINATION lib/cmake/file
)
