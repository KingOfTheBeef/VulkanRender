//
// Created by jonat on 22/05/2020.
//

#ifndef DYNAMICLINK_CORE_H
#define DYNAMICLINK_CORE_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include <vector>

#include "WindowContext.h"

struct SwapchainInfo {
    VkSwapchainKHR swapchain;
    uint32_t imageCount;
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

class Core {
private:
    VKLIB vulkanLib;
    VkInstance instance;
    DeviceInfo deviceInfo;
    WindowContext *windowContext;
    VkSurfaceKHR surface;

    SwapchainInfo swapchainInfo;

    // Think about putting cmd buffer stuff in a struct

    VkCommandPool cmdPool;
    uint32_t cmdBufferCount;
    // TODO: Need to add code to free memory allocated
    VkCommandBuffer *cmdBuffers;

    VkSemaphore imageAvailableSema;
    VkSemaphore imageFinishProcessingSema;
public:
    Core();
    void init();
    void draw();
    void clean();


private:
    void populateLayerInfo(VkInstanceCreateInfo *info);
    int populateExtensionInfo(VkInstanceCreateInfo *info);
    void populateQueueCreateInfo(VkDeviceQueueCreateInfo *queueCreateInfo, uint32_t queueFamilyIndex, const float *queuePriorities);
    void initDevice();
    void initSurface();
    void initWindowContext();

    int selectPhysicalDevice();

    bool checkPhysicalDeviceQueues(VkPhysicalDevice physicalDevice, uint32_t *queueIndices);
    bool checkPhysicalDeviceExtensions(VkPhysicalDevice physicalDevice, const uint8_t targetExtensionCount, const char **targetExtensions);

    void initSwapchain();
    void initExtent2D(VkExtent2D *extent, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int initPresentMode(VkPresentModeKHR *presentMode);
    uint32_t getImageCount(VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int initImageUsageFlags(VkImageUsageFlags *usageFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int initSurfaceFormat(VkSurfaceFormatKHR *surfaceFormat);
    int initPretransform(VkSurfaceTransformFlagBitsKHR *transformFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int initCommandBuffers();
    int recordCommandBuffers();
    int initSemaphores();
};

#endif //DYNAMICLINK_CORE_H
