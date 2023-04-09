cmake_minimum_required(VERSION 3.16)

project(Assembly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -Wall)

set(CURRENT_EXECUTABLE "${PROJECT_NAME}")

add_executable(
    Assembly
    ${Assembly_SOURCE_DIR}/main.cpp
    ${Assembly_SOURCE_DIR}/assembler/handler.cpp
    ${Assembly_SOURCE_DIR}/interpreter/handler.cpp
    ${Assembly_SOURCE_DIR}/interpreter/state.cpp
    ${Assembly_SOURCE_DIR}/disassembler/handler.cpp
    ${Assembly_SOURCE_DIR}/common/config.cpp
    ${Assembly_SOURCE_DIR}/common/bytecode_state.cpp
)

target_include_directories(
    Assembly PUBLIC
    ${Assembly_SOURCE_DIR}
)
