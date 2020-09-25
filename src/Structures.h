//
// Created by jonat on 08/07/2020.
//

#ifndef DYNAMICLINK_STRUCTURES_H
#define DYNAMICLINK_STRUCTURES_H

#define NO_PROTOTYPES
#include <vulkan/vulkan.h>

struct Image {
    VkImage     handle;
    VkImageView view;
    VkFormat    format;
    uint32_t    offset;
};

struct CombinedImageSampler {
    Image image;
    VkSampler sampler;
};

struct DescriptorSet {
    VkDescriptorSet         handle;
    VkDescriptorSetLayout   layout;
    VkDescriptorPool        pool;
};

struct BinaryFile {
    uint32_t size;
    char *data;
};

struct ImageFile : BinaryFile {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
};

struct VirtualFrame {
    VkCommandBuffer cmdBuffer;
    VkSemaphore imageAvailableSema;
    VkSemaphore imageFinishProcessingSema;
    VkFramebuffer framebuffer;
    VkFence fence;
};

/*
struct SwapchainInfo {
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR imageFormat;
    VkExtent2D extent;
    uint32_t imageCount;
    VkImage *images;
    VkImageView *imageViews;
};
 */

struct DeviceInfo {
    VkPhysicalDevice physical;
    VkDevice logical;
    uint32_t graphicQueueIndex;
    VkQueue graphicQueue;
    uint32_t displayQueueIndex;
    VkQueue displayQueue;
};

#endif //DYNAMICLINK_STRUCTURES_H
