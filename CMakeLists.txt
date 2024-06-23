cmake_minimum_required(VERSION 3.14)
project(DynamicLink)

if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    add_compile_definitions(PLATFORM="${CMAKE_SYSTEM_NAME}")
    add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
endif()

add_subdirectory(src)
