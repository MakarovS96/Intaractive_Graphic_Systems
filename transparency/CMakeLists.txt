cmake_minimum_required(VERSION 3.8)
project(HelloTriangle)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp assimp_mloader/Mesh.h assimp_mloader/Model.h camera/Camera.h libstbimage/stb_image.h)
set(GLAD_LIB glad/glad.c)
#set(CAM_LIB camera/Camera.cpp)
#set(MODEL_LIB assimp_mloader/Mesh.cpp assimp_mloader/Model.cpp)
set(SHADER_LOADER_LIB libshader/Shader.cpp)
set(SYSTEM_LIB GL glfw3 pthread Xi dl X11 Xinerama Xcursor Xxf86vm Xrandr rt m assimp)

include_directories("${PROJECT_BINARY_DIR}/include")

add_library(shader STATIC ${SHADER_LOADER_LIB})
add_library(glad STATIC ${GLAD_LIB})
#add_library(cam STATIC ${CAM_LIB})
#add_library(model STATIC ${MODEL_LIB})

add_executable(HelloTriangle ${SOURCE_FILES})

target_link_libraries(HelloTriangle shader)
target_link_libraries(HelloTriangle glad)
#target_link_libraries(HelloTriangle cam)
#target_link_libraries(HelloTriangle model)
target_link_libraries(HelloTriangle ${SYSTEM_LIB})