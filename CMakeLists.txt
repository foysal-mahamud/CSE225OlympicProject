cmake_minimum_required(VERSION 3.10)
project(olympic CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

file(GLOB OLYMPIC_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(olympic ${OLYMPIC_SOURCES})
target_include_directories(olympic PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
