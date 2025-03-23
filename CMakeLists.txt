cmake_minimum_required(VERSION 3.7)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin/$<CONFIG>/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin/$<CONFIG>/${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin/$<CONFIG>/${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

option(CL_BUILD_EXAMPLE "Build CLog with the example" ON)

project(CLog VERSION 1.0)

add_library(CLog STATIC
    CLog.c
)

if(CL_BUILD_EXAMPLE)
    add_executable(CLogExample
        CLogExample.c
    )

    target_link_libraries(CLogExample
        PRIVATE CLog
    )
endif()

