cmake_minimum_required(VERSION 3.16)
project(atom)
set(CMAKE_C_STANDARD 99)
set(ATOM_COMPILER_NAME atomc)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCE)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/util UTIL)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/engine ENGINE)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/engine/statement ENGINE_STATEMENT)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/engine/expression ENGINE_EXPRESSION)

aux_source_directory(${PROJECT_SOURCE_DIR}/include HEADERS)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(
        -D_DEBUG
    )
    aux_source_directory(${PROJECT_SOURCE_DIR}/src/debug DEBUG)
endif()

add_executable(${ATOM_COMPILER_NAME} ${SOURCE} ${DEBUG} ${UTIL} ${ENGINE} ${ENGINE_STATEMENT} ${ENGINE_EXPRESSION})
