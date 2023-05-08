cmake_minimum_required(VERSION 3.16)

project(Futures)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -Wall)

set(CURRENT_EXECUTABLE "${PROJECT_NAME}")

add_executable(
    Futures
    ${Futures_SOURCE_DIR}/main.cpp
)

target_include_directories(
    Futures PUBLIC
    ${Futures_SOURCE_DIR}
)
