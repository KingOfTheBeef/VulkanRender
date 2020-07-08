//
// Created by jonat on 05/07/2020.
//

#ifndef DYNAMICLINK_RENDERER_H
#define DYNAMICLINK_RENDERER_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>

class Renderer {
public:
    int initRenderPass(VkDevice device, VkFormat format);
    int initFramebuffers(VkDevice device, uint32_t imageViewCount, VkImageView *imageViews);
    int initShaderModule(VkDevice device, const char* filename, VkShaderModule *shaderModule);
    int initGraphicPipeline(VkDevice device);
    void clean();
private:
    VkPipeline pipeline;
    VkRenderPass renderPass;
    VkFramebuffer *framebuffers;
};

#endif //DYNAMICLINK_RENDERER_H
