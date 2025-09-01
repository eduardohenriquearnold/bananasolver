cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Bananagrams)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(bananagrams src/main.cpp src/dictionary.cpp src/board.cpp)

# Include directories
target_include_directories(bananagrams PRIVATE src)