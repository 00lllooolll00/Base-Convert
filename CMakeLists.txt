cmake_minimum_required(VERSION 3.20)

project(base convert)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(bconv src/main.c)

