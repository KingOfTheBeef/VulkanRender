cmake_minimum_required(VERSION 3.14)
project(DynamicLink)

#set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../VulkanSDK/1.2.135.0/Include)
include_directories(../../SDL2-2.0.9/include)

set(SDL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../SDL2-2.0.9/lib/x86/)
find_library(SDL2 SDL2.lib PATH ${SDL_LIB_PATH})
find_library(SDL2main SDL2main.lib PATH ${SDL_LIB_PATH})

if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    add_compile_definitions(PLATFORM="${CMAKE_SYSTEM_NAME}")
    add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
endif()

add_subdirectory(src)
