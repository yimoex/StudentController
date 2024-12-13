cmake_minimum_required(VERSION 3.10)
project(StudentController)

set(CMAKE_C_STANDARD 99)

file(GLOB HEADERS
    includes/*.h
    src/platform/encoder/*.h
    src/platform/networker/*.h
)

include_directories(${CMAKE_SOURCE_DIR}/includes)

file(GLOB SOURCES
    src/libs/*.c
    src/platform/index.c
    src/main.c
)

add_executable(StudentController ${SOURCES})

target_link_libraries(StudentController ws2_32)