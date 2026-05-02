cmake_minimum_required(VERSION 3.14)

# CXX Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project
project(rpi-pms7003)

# Link
add_executable(rpi-finedust)

target_sources(rpi-finedust PRIVATE src/main.cpp)

target_include_directories(rpi-finedust PRIVATE include)

# Custom Targets
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  add_custom_target(
    copy_compile_commands ALL
    COMMAND
      ${CMAKE_COMMAND} -E create_symlink
      "${CMAKE_BINARY_DIR}/compile_commands.json"
      "${CMAKE_SOURCE_DIR}/compile_commands.json"
    DEPENDS "${CMAKE_BINARY_DIR}/compile_commands.json"
    COMMENT "Creating symlink for compile_commands.json in source dir")
endif()
