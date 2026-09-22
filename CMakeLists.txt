cmake_minimum_required(VERSION 3.20)

project(WoLume)

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders/bin)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED CONFIG)
find_program(SLANGC slangc REQUIRED)

file(GLOB SHADER_SOURCES CONFIGURE_DEPENDS
        ${SHADER_DIR}/*.slang
)

set(SHADER_OUTPUTS)

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)

    set(SHADER_OUTPUT
            ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv
    )

    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}

        COMMAND ${CMAKE_COMMAND} -E make_directory
        ${SHADER_OUTPUT_DIR}

        COMMAND ${SLANGC}
        ${SHADER}
        -target spirv
        -o ${SHADER_OUTPUT}

        DEPENDS ${SHADER}
    )

    list(APPEND SHADER_OUTPUTS ${SHADER_OUTPUT})
endforeach()

add_custom_target(shaders
    DEPENDS ${SHADER_OUTPUTS}
)

add_executable(renderer
    src/main.cpp
    src/renderer.cpp
    src/renderer_core.cpp
)

target_compile_definitions(renderer PRIVATE
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

add_dependencies(renderer shaders)

target_compile_features(renderer PRIVATE cxx_std_23)

target_link_libraries(renderer
    PRIVATE
    Vulkan::Vulkan
    SDL3::SDL3
)

target_include_directories(renderer
    PRIVATE
    include
)