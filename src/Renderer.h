//
// Created by jonat on 05/07/2020.
//

#ifndef DYNAMICLINK_RENDERER_H
#define DYNAMICLINK_RENDERER_H

#include <vulkan/vulkan.h>

class Renderer {
public:
    int initRenderPass(VkDevice device, VkFormat format);

private:
    VkRenderPass renderPass;
};

#endif //DYNAMICLINK_RENDERER_H
