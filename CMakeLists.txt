cmake_minimum_required(VERSION 3.20)

project(khr_vulkan_tut
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_program(SLANGC_EXECUTABLE slangc REQUIRED)

function(add_slang_shader_target TARGET)
    cmake_parse_arguments(
            SHADER
            ""
            ""
            "SOURCES"
            ${ARGN}
    )

    set(SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

    add_custom_command(
            OUTPUT ${SHADERS_DIR}/slang.spv

            COMMAND ${CMAKE_COMMAND} -E make_directory
            ${SHADERS_DIR}

            COMMAND ${SLANGC_EXECUTABLE}
            ${SHADER_SOURCES}
            -target spirv
            -profile spirv_1_4
            -emit-spirv-directly
            -fvk-use-entrypoint-name
            -entry vertMain
            -entry fragMain
            -o ${SHADERS_DIR}/slang.spv

            DEPENDS ${SHADER_SOURCES}

            COMMENT "Compiling Slang shaders"
            VERBATIM
    )

    add_custom_target(${TARGET}
            DEPENDS ${SHADERS_DIR}/slang.spv
    )
endfunction()


add_executable(${PROJECT_NAME}
        src/main.cpp
        src/config.hpp
        src/app.cpp
        src/instance.cpp
        src/window.cpp
        src/debug_messenger.cpp
        src/device.cpp
        src/swapchain.cpp
        src/pipeline.cpp
        src/command_buffers.cpp
        src/renderer.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Vulkan::Vulkan
        glfw
)


set(SHADER_SLANG_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/basic_shader.slang
)

add_slang_shader_target(shader
        SOURCES ${SHADER_SLANG_SOURCES}
)

add_dependencies(${PROJECT_NAME} shader)