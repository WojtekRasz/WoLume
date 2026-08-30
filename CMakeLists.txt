cmake_minimum_required(VERSION 3.20)

project(khr_vulkan_tut
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES}
        src/config.hpp
        src/window.cpp
        src/window.hpp
        src/debug_messenger.cpp
        src/debug_messenger.hpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Vulkan::Vulkan
        glfw
)