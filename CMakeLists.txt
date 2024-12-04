cmake_minimum_required(VERSION 3.25)

project(dbscan)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

add_compile_options(
  -Wall
  -Wextra
  -Wpedantic
  -pedantic
  -Werror
  -fconcepts-diagnostics-depth=10
  # Asan
  $<$<CONFIG:Asan>:-fsanitize=undefined,address>
  $<$<CONFIG:Asan>:-O0>
  $<$<CONFIG:Asan>:-g>
  # Tsan
  $<$<CONFIG:Tsan>:-fsanitize=thread>
  $<$<CONFIG:Tsan>:-O1>
  $<$<CONFIG:Tsan>:-g>
)
link_libraries(
  # Asan
  $<$<CONFIG:Asan>:asan>
  $<$<CONFIG:Asan>:ubsan>
  # Tsan
  $<$<CONFIG:Tsan>:tsan>
)

add_subdirectory(src)
