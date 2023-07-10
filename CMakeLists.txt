cmake_minimum_required(VERSION 3.16)
project(atom)
set(CMAKE_C_STANDARD 99)
set(ATOM_COMPILER_NAME atomc)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCE)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/util UTIL)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/parser PARSER)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/compiler COMPILER)

aux_source_directory(${PROJECT_SOURCE_DIR}/include HEADERS)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(
        -D_DEBUG
    )
    aux_source_directory(${PROJECT_SOURCE_DIR}/src/debug DEBUG)
endif()

add_executable(${ATOM_COMPILER_NAME} ${SOURCE} ${DEBUG} ${UTIL} ${PARSER} ${COMPILER})
