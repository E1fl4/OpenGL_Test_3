cmake_minimum_required(VERSION 3.10)
project(OpenGL_Test_3 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
link_directories(lib)

set(SOURCES
    src/main.cpp
    src/glad.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    glfw3
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)
