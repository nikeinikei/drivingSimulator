cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(drivingSimulator)

find_package(Torch REQUIRED)

add_library(GLAD_STATIC STATIC libs/glad/src/glad.c)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw-3.3.4)

file(GLOB IMGUI_HEADERS libs/imgui/*.h)
file(GLOB IMGUI_SOURCES libs/imgui/*.cpp)
add_library(IMGUI_STATIC STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_link_libraries(IMGUI_STATIC glfw GLAD_STATIC)

add_subdirectory(libs/glm-0.9.9.8)

add_executable(drivingSimulator main.cpp Application.cpp Application.h GameState.h SimulationGameState.h Scene.cpp Scene.h tiny_obj_loader.h FreeCamera.cpp FreeCamera.h AbstractCamera.h)
include_directories(drivingSimulator libs/imgui libs/glad/include)
target_link_libraries(drivingSimulator
        IMGUI_STATIC
        ${TORCH_LIBRARIES}
        glfw
        glm::glm)

set_property(TARGET drivingSimulator PROPERTY CXX_STANDARD 17)
