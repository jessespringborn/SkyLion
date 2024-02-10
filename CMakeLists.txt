cmake_minimum_required(VERSION 3.27)
project(SkyLion C)

set(CMAKE_C_STANDARD 99)

# include directories
include_directories(include)

add_subdirectory(third_party/sdl EXCLUDE_FROM_ALL)

add_subdirectory(third_party/cimgui EXCLUDE_FROM_ALL)

add_subdirectory(third_party/cglm/ EXCLUDE_FROM_ALL)

add_executable(SkyLion
        src/Window.c
        src/Window.h
        src/SkyLion.c
        src/SkyLion.h
        src/Renderer.c
        src/Renderer.h
        src/Model.c
        src/Model.h
)

target_link_libraries(SkyLion PRIVATE SDL3::SDL3)

target_include_directories(SkyLion PUBLIC third_party/vma/include)

target_include_directories(SkyLion PUBLIC third_party/cgltf)

target_include_directories(SkyLion PUBLIC third_party/cimgui)

target_include_directories(SkyLion PUBLIC third_party/cglm/include)

# find and link vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(SkyLion PRIVATE Vulkan::Vulkan)

############## Build SHADERS #######################
# Try to find glslc in the project directory
find_program(GLSLC_EXECUTABLE NAMES glslc PATHS ${PROJECT_SOURCE_DIR}/third_party/vulkan/bin NO_DEFAULT_PATH)

# Fallback to system search if not found in the project
if(NOT GLSLC_EXECUTABLE)
    find_program(GLSLC_EXECUTABLE NAMES glslc)
endif()

if(NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found. Please provide the path to glslc.")
endif()

# Now GLSLC_EXECUTABLE variable holds the path to the glslc executable

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

file(GLOB_RECURSE GLSL_SOURCE_FILES "${SHADER_SOURCE_DIR}/*.vert" "${SHADER_SOURCE_DIR}/*.frag")
foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL}
            COMMENT "Compiling ${GLSL} to ${SPIRV}"
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(SkyLion CompileShaders)

