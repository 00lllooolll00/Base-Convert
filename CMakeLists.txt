cmake_minimum_required(VERSION 3.20)

project(convert)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC)

include_directories( INC
    ${CMAKE_SOURCE_DIR}/inc
)

add_compile_definitions(DEF
    DEBUG
)

add_executable(bconv ${SRC})



