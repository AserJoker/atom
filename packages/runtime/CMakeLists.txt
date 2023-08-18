cmake_minimum_required(VERSION 3.12)
project(${PACKAGE_NAME})

LoadModules(modules ${PROJECT_SOURCE_DIR})

add_library(${PROJECT_NAME} ${modules})
