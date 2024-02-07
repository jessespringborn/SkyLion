cmake_minimum_required(VERSION 3.27)
project(SkyLion C)

set(CMAKE_C_STANDARD 99)

# include directories
include_directories(include)

add_subdirectory(third_party/sdl EXCLUDE_FROM_ALL)

add_subdirectory(third_party/cimgui EXCLUDE_FROM_ALL)

add_executable(SkyLion
        src/Window.c
        src/Window.h
        src/SkyLion.c
        src/SkyLion.h
        src/Renderer.c
        src/Renderer.h
)

target_link_libraries(SkyLion PRIVATE SDL3::SDL3)

target_include_directories(SkyLion PUBLIC third_party/vma/include)

target_include_directories(SkyLion PUBLIC third_party/cgltf)

target_include_directories(SkyLion PUBLIC third_party/cimgui)

# find and link vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(SkyLion PRIVATE Vulkan::Vulkan)
