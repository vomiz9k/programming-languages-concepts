cmake_minimum_required(VERSION 3.16)

project(LINQ)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -Wall)

set(CURRENT_EXECUTABLE "${PROJECT_NAME}")

add_executable(
    LINQ
    ${LINQ_SOURCE_DIR}/main.cpp
)

target_include_directories(
    LINQ PUBLIC
    ${LINQ_SOURCE_DIR}
)