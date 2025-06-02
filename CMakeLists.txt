cmake_minimum_required(VERSION 3.31)
project(z1000)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)


add_executable(z1000 src/main.cpp)

target_link_libraries(z1000 glfw Vulkan::Vulkan)