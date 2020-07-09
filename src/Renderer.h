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
    int initRenderPass(VkDevice device, VkFormat format);

    int initFramebuffers(VkDevice device, uint32_t imageViewCount, VkImageView *imageViews);

    int initShaderModule(VkDevice device, const char *filename, VkShaderModule *shaderModule);

    int initGraphicPipeline(DeviceInfo device);

    int initCommandBuffers(DeviceInfo device, uint32_t bufferCount);

    int recordCommandBuffers(DeviceInfo device, VkImage *images);

    void clean(DeviceInfo device);

    VkCommandBuffer *getCmdBuffers() { return this->cmdBuffers; };

private:
    VkPipeline pipeline;
    VkRenderPass renderPass;

    uint32_t framebufferCount;
    VkFramebuffer *framebuffers;

    VkCommandPool cmdPool;
    uint32_t cmdBufferCount;
    VkCommandBuffer *cmdBuffers;
};

#endif //DYNAMICLINK_RENDERER_H
