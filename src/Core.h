//
// Created by jonat on 22/05/2020.
//

#ifndef DYNAMICLINK_CORE_H
#define DYNAMICLINK_CORE_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>

#include "WindowContext.h"

struct Device {
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
    Device device;
    WindowContext *windowContext;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
public:
    Core();
    void init();
    void clean();


private:
    void populateLayerInfo(VkInstanceCreateInfo &info);
    void initDevice();
    void initSurface();
    void initWindowContext();
    bool validatePhysicalDevice(VkPhysicalDevice physicalDevice, uint32_t queueIndices[], uint8_t targetExtensionCount, const char* targetExtensions[]);
    void initSwapchain();
    void initExtent2D(VkExtent2D *extent, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int initPresentMode(VkPresentModeKHR *presentMode);
    uint32_t getImageCount(VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int initImageUsageFlags(VkImageUsageFlags *usageFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int initSurfaceFormat(VkSurfaceFormatKHR *surfaceFormat);
};

#endif //DYNAMICLINK_CORE_H
