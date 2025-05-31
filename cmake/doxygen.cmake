
find_package(Doxygen REQUIRED)

doxygen_add_docs(
  doxygen
  ${CMAKE_CURRENT_SOURCE_DIR}/include 
  CONFIG_FILE 
  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
)
