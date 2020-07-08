cmake_minimum_required(VERSION 3.14)
project(DynamicLink)

set(CMAKE_CXX_STANDARD 14)

add_executable(DynamicLink
        main.cpp
        VulkanInit.cpp
        Core.cpp WindowContext.h WindowContext.cpp Renderer.h Renderer.cpp FileReader.h FileReader.cpp Structures.h)

target_link_libraries(DynamicLink ${SDL2} ${SDL2main})
