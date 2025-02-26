cmake_minimum_required(VERSION 3.16)
project(cmd_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
include(CTest)

# Fetch dependencies (if needed)
include(FetchContent)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)
