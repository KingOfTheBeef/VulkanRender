//
// Created by jonat on 05/07/2020.
//

#ifndef DYNAMICLINK_RENDERER_H
#define DYNAMICLINK_RENDERER_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>

#include "Structures.h"

struct VirtualFrame {
    VkCommandBuffer cmdBuffer;
    VkSemaphore imageAvailableSema;
    VkSemaphore imageFinishProcessingSema;
    VkFramebuffer framebuffer;
    VkFence fence;
};

class Renderer {
public:
    int initRenderPass(VkDevice device, VkFormat format);

    // int initFramebuffers(VkDevice device, uint32_t imageViewCount, VkImageView *imageViews);

    int initShaderModule(VkDevice device, const char *filename, VkShaderModule *shaderModule);

    int initGraphicPipeline(DeviceInfo device);

    int initVirtualFrames(DeviceInfo device);

    // int initCommandBuffers(DeviceInfo device, uint32_t bufferCount);

    int initVertexBuffer(DeviceInfo device);

    // int recordCommandBuffers(DeviceInfo device, VkImage *images);

    int prepareVirtualFrame(DeviceInfo device, VirtualFrame *virtualFrame, VkExtent2D extent, VkImageView *imageView, VkImage image);

    void draw(DeviceInfo device, SwapchainInfo swapchain);

    void clean(DeviceInfo device);

private:
    VkPipeline pipeline;
    VkRenderPass renderPass;

    VkCommandPool cmdPool;
    // uint32_t cmdBufferCount;
    // VkCommandBuffer *cmdBuffers;

    static const uint32_t virtualFrameCount = 3;
    VirtualFrame virtualFrames[virtualFrameCount];

    VkBuffer vertexBuffer;
    VkDeviceMemory  deviceMemory;
};

#endif //DYNAMICLINK_RENDERER_H
