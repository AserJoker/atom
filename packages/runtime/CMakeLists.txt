cmake_minimum_required(VERSION 3.12)
project(${PACKAGE_NAME})
aux_source_directory(src ${PROJECT_NAME}-src)
add_library(${PROJECT_NAME} ${${PROJECT_NAME}-src})
