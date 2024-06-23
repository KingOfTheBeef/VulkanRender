cmake_minimum_required(VERSION 3.14)
project(DynamicLink)

set(CMAKE_CXX_STANDARD 14)

add_custom_target(copy_shader_modules)
add_custom_command(
        TARGET copy_shader_modules POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders)

add_custom_target(copy_images)
add_custom_command(
        TARGET copy_images POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/img
        ${CMAKE_CURRENT_BINARY_DIR}/img)


find_package(SDL2 CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)
find_package(Stb REQUIRED)

add_executable(DynamicLink
        main.cpp
        VulkanInit.cpp
        Core.cpp WindowContext.h WindowContext.cpp Renderer.h Renderer.cpp FileReader.h FileReader.cpp Structures.h tempVertexData.h Swapchain.cpp Swapchain.h GraphicMath.h Buffer.cpp Buffer.h DeviceMemory.cpp DeviceMemory.h VulkanStructures.h)

target_link_libraries(DynamicLink PRIVATE SDL2::SDL2 SDL2::SDL2main Vulkan::Headers ${Stb_INCLUDE_DIR})
add_dependencies(DynamicLink copy_shader_modules copy_images)
