cmake_minimum_required(VERSION 3.16)
project(delayfmu CXX C)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Check that compiler supports it
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE C_FILES src/*.c)
add_library(delayfmu SHARED
        ${CPP_FILES}
        ${C_FILES})

target_include_directories(delayfmu PRIVATE src/fmi2_0_2-headers)
