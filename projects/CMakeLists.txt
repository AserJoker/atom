cmake_minimum_required(VERSION 3.12)
ListDirectories(projects ${PROJECT_SOURCE_DIR}/projects)

foreach(project ${projects})
    message(NOTICE "find project ${project}")
    add_subdirectory(${project})
endforeach()
