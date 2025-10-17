cmake_minimum_required(VERSION 3.23)

project(File CXX)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 2)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

option(FILE_BUILD_TESTS "" ${PROJECT_IS_TOP_LEVEL})
option(FILE_BUILD_DOCS  "" ${PROJECT_IS_TOP_LEVEL})

add_library(File INTERFACE)
add_library(File::File ALIAS File)

target_compile_features(File INTERFACE cxx_std_17)
target_sources(File
    INTERFACE
        FILE_SET HEADERS BASE_DIRS
            headers
        FILES
            headers/wqfile/file.h
)
if(FILE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(FILE_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    FetchContent_Declare(
        doxygen-awesome-css
        GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css
        GIT_TAG 9760c30014131f4eacb8e96f15f3869c7bc5dd8c
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)
    FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

    set(DOXYGEN_PROJECT_NAME File)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_DISABLE_INDEX     NO)
    set(DOXYGEN_FULL_SIDEBAR      NO)
    set(DOXYGEN_HTML_COLORSTYLE   LIGHT)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${AWESOME_CSS_DIR}/doxygen-awesome.css)
    doxygen_add_docs(FileDocs
        ${CMAKE_CURRENT_SOURCE_DIR}/headers/wqfile ALL
    )
endif()

if(PROJECT_IS_TOP_LEVEL)
    install(TARGETS File
            EXPORT  FileConfig FILE_SET HEADERS)
    install(EXPORT    FileConfig
            NAMESPACE File:: DESTINATION lib/cmake/File)
endif()
