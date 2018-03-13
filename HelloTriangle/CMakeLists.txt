cmake_minimum_required(VERSION 3.8)
project(HelloTriangle)

set(CMAKE_CXX_STANDARD 11)
set(DEBUG 1)

configure_file("${PROJECT_SOURCE_DIR}/shaderIzi/shaderIzi.h"
                "${PROJECT_BINARY_DIR}/include/shader_loader.h")

set(SOURCE_FILES main.cpp)
set(GLAD_LIB glad/glad.c)
set(SHADER_LOADER_LIB shaderIzi/shaderIzi.cpp)
set(SYSTEM_LIB GL glfw3 pthread Xi dl X11 Xinerama Xcursor Xxf86vm Xrandr rt m)

include_directories("${PROJECT_BINARY_DIR}/include")

add_library(shaderIzi STATIC ${SHADER_LOADER_LIB})
add_library(glad STATIC ${GLAD_LIB})

add_executable(HelloTriangle ${SOURCE_FILES})

target_link_libraries(HelloTriangle shaderIzi)
target_link_libraries(HelloTriangle glad)
target_link_libraries(HelloTriangle ${SYSTEM_LIB})