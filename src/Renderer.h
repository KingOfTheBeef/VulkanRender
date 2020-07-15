//
// Created by jonat on 05/07/2020.
//

#ifndef DYNAMICLINK_RENDERER_H
#define DYNAMICLINK_RENDERER_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include "Structures.h"

class Renderer {
public:
    int initRenderer(DeviceInfo device, VkFormat format);

    int updateVertexBuffer(DeviceInfo device, const void* data, size_t size);

    int draw(DeviceInfo device, SwapchainInfo swapchain);

    void clean(DeviceInfo device);

private:
    VkPipeline              pipeline;
    VkRenderPass            renderPass;
    VkCommandPool           cmdPool;
    static const uint32_t   virtualFrameCount = 3;
    VirtualFrame            virtualFrames[virtualFrameCount];
    VkBuffer                vertexBuffer;
    VkDeviceMemory          hostVisibleMemory;
    VkDeviceMemory          deviceLocalMemory;

private:
    int initRenderPass(VkDevice device, VkFormat format);

    int allocateDeviceMemory(DeviceInfo device, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceMemory *memory);

    int initBuffer(DeviceInfo device, VkBufferUsageFlags bufferUsageFlags, size_t size, VkBuffer *buffer);

    int initShaderModule(VkDevice device, const char *filename, VkShaderModule *shaderModule);

    int initGraphicPipeline(DeviceInfo device);

    int initVirtualFrames(DeviceInfo device);

    int initCommandPool(DeviceInfo device);

    int initVertexBuffer(DeviceInfo device);

    int prepareVirtualFrame(DeviceInfo device, VirtualFrame *virtualFrame, VkExtent2D extent, VkImageView *imageView, VkImage image);
};

#endif //DYNAMICLINK_RENDERER_H
