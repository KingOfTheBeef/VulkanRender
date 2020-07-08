//
// Created by jonat on 08/07/2020.
//

#ifndef DYNAMICLINK_STRUCTURES_H
#define DYNAMICLINK_STRUCTURES_H

#define NO_PROTOTYPES
#include <vulkan/vulkan.h>

struct SwapchainInfo {
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR imageFormat;
    uint32_t imageCount;
    VkImage *images;
    VkImageView *imageViews;
};

struct DeviceInfo {
    VkPhysicalDevice physical;
    VkDevice logical;
    uint32_t graphicQueueIndex;
    VkQueue graphicQueue;
    uint32_t displayQueueIndex;
    VkQueue displayQueue;
};

#endif //DYNAMICLINK_STRUCTURES_H
