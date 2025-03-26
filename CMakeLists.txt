cmake_minimum_required(VERSION 3.7)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

string(TOLOWER ${CMAKE_BUILD_TYPE} CLOG_BUILD_TYPE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CLOG_BUILD_TYPE}/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CLOG_BUILD_TYPE}/${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CLOG_BUILD_TYPE}/${PROJECT_NAME})

option(CLOG_BUILD_EXAMPLE "Build CLog with the example" ON)
option(CLOG_USE_ASAN "Build clog with Adress Sanitizer" ON)

set(CLOG_ASAN_LIB "")
set(CLOG_ASAN_COMPILE_OPTIONS "")

if(${CLOG_USE_ASAN})
    set(CLOG_ASAN_LIB asan)
    set(CLOG_ASAN_COMPILE_OPTIONS -fsanitize=address)
endif()

project(clog VERSION 1.0)

add_library(clog STATIC
    clog.c
)

target_link_libraries(clog
    PRIVATE ${CLOG_ASAN_LIB}
)

target_compile_options(clog
    PRIVATE ${CLOG_ASAN_COMPILE_OPTIONS}
)

if(${CLOG_BUILD_EXAMPLE})
    add_executable(clog_example
        clog_example.c
    )

    target_link_libraries(clog_example
        PRIVATE clog
        PRIVATE ${CLOG_ASAN_LIB}
    )

    target_compile_options(clog_example
        PRIVATE ${CLOG_ASAN_COMPILE_OPTIONS}
    )
endif()

