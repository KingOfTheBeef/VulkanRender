//
// Created by jonat on 22/05/2020.
//

#ifndef DYNAMICLINK_CORE_H
#define DYNAMICLINK_CORE_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include <vector>

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
    int initPretransform(VkSurfaceTransformFlagBitsKHR *transformFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int initCommandBuffers();
    int recordCommandBuffers();
    int initSemaphores();
};

#endif //DYNAMICLINK_CORE_H
