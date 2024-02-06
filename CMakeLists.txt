cmake_minimum_required(VERSION 3.27)
project(SkyLion C)

set(CMAKE_C_STANDARD 23)

add_subdirectory(external/sdl EXCLUDE_FROM_ALL)

add_subdirectory(external/cimgui EXCLUDE_FROM_ALL)

add_executable(SkyLion main.c)

target_link_libraries(SkyLion PRIVATE SDL3::SDL3)

target_include_directories(SkyLion PUBLIC external/vma/include)

target_include_directories(SkyLion PUBLIC external/cimgui)