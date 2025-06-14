cmake_minimum_required(VERSION 3.31)
project(z1000)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)


FetchContent_Declare(
        tinyobjloader
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
        GIT_TAG        v2.0.0rc13  # or use main for latest
)
FetchContent_MakeAvailable(tinyobjloader)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.91.9b
)
FetchContent_MakeAvailable(imgui)


set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

add_library(imgui_lib ${IMGUI_SOURCES})

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)


add_executable(z1000 src/main.cpp)

target_include_directories(imgui_lib PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui_lib glfw Vulkan::Vulkan)

target_link_libraries(z1000 glfw Vulkan::Vulkan tinyobjloader nlohmann_json::nlohmann_json imgui_lib)
target_include_directories(z1000 PRIVATE ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)