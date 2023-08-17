cmake_minimum_required(VERSION 3.12)
project(atomc.out)
aux_source_directory(src ${PROJECT_NAME}-src)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}-src})
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

foreach(package ${packages})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ROOT_PROJECT_NAME}.${package})
endforeach()

