//
// Created by jonat on 22/05/2020.
//

#ifndef DYNAMICLINK_CORE_H
#define DYNAMICLINK_CORE_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include <vector>

#include "Structures.h"
#include "WindowContext.h"
#include "Renderer.h"

class Core {
private:
    VKLIB vulkanLib;
    VkInstance instance;
    DeviceInfo deviceInfo;
    WindowContext *windowContext;
    VkSurfaceKHR surface;

    SwapchainInfo swapchainInfo;

    Renderer renderer;

    // VkSemaphore imageAvailableSema;
    // VkSemaphore imageFinishProcessingSema;
public:
    Core();

    void      init();
    void      draw();
    void      clean();
    void      windowResize();
    void      update();


private:
    void      populateLayerInfo(VkInstanceCreateInfo *info);
    int       populateExtensionInfo(VkInstanceCreateInfo *info);
    void      populateQueueCreateInfo(VkDeviceQueueCreateInfo *queueCreateInfo, uint32_t queueFamilyIndex, const float *queuePriorities);
    void      initDevice();
    void      initSurface();
    void      initWindowContext();

    int       selectPhysicalDevice();

    bool      checkPhysicalDeviceQueues(VkPhysicalDevice physicalDevice, uint32_t *queueIndices);
    bool      checkPhysicalDeviceExtensions(VkPhysicalDevice physicalDevice, const uint8_t targetExtensionCount, const char **targetExtensions);

    void      initSwapchain();
    void      initExtent2D(VkExtent2D *extent, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int       initPresentMode(VkPresentModeKHR *presentMode);
    uint32_t  getImageCount(VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int       initImageUsageFlags(VkImageUsageFlags *usageFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int       initSurfaceFormat(VkSurfaceFormatKHR *surfaceFormat);
    int       initPretransform(VkSurfaceTransformFlagBitsKHR *transformFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    // int initSemaphores();
    int       initSwapchainImages();
};

#endif //DYNAMICLINK_CORE_H
