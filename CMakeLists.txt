cmake_minimum_required(VERSION 3.15)
project(OpenGL_Test_3)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_PATH "/opt/homebrew/Cellar/glfw/3.4")
set(GLEW_PATH "/opt/homebrew/Cellar/glew/2.2.0_1")
set(GLM_PATH "/opt/homebrew/Cellar/glm/1.0.1")

include_directories(
    ${GLFW_PATH}/include
    ${GLEW_PATH}/include
    ${GLM_PATH}/include
)

link_directories(
    ${GLFW_PATH}/lib
    ${GLEW_PATH}/lib
)

set(SOURCES src/main.cpp)

add_compile_definitions(GL_SILENCE_DEPRECATION)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glew)
target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
target_link_libraries(${PROJECT_NAME} "-framework IOKit")
target_link_libraries(${PROJECT_NAME} "-framework CoreVideo")


