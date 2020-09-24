//
// Created by jonat on 20/08/2020.
//

#ifndef DYNAMICLINK_SWAPCHAIN_H
#define DYNAMICLINK_SWAPCHAIN_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include "Structures.h"

// this->swapchainInfo = {VK_NULL_HANDLE, {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}, {0, 0}, 0, nullptr};
class Swapchain {
private:
    VkSwapchainKHR      handle;
    VkSurfaceFormatKHR  surfaceFormat;
    VkExtent2D          extent;
    uint32_t            imageCount;
    VkImage             *images;
    VkImageView         *imageViews;

public:
    Swapchain();

    void      initSwapchain(DeviceInfo device, VkSurfaceKHR surface);

    void      recreateSwapchain(DeviceInfo device, VkSurfaceKHR surface);

    VkResult  acquireImage(DeviceInfo device, uint32_t *imageIndex, VkSemaphore imageAvailableSemaphore);
    VkResult  presentImage(DeviceInfo device, uint32_t imageIndex, VkSemaphore imageFinishProcessingSemaphore);

    uint32_t            getImageCount()     { return this->imageCount; };
    VkImage*            getImages()         { return this->images; };
    VkImageView*        getImageViews()     { return this->imageViews; };
    VkExtent2D          getExtent()         { return this->extent; };
    VkSwapchainKHR      getHandle()         { return this->handle; };
    VkSurfaceFormatKHR  getSurfaceFormat()  { return this->surfaceFormat; };

    void      clean(DeviceInfo device);

private:
    void      initExtent2D(VkExtent2D *extent, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int       initPresentMode(DeviceInfo device, VkSurfaceKHR surface, VkPresentModeKHR *presentMode);
    uint32_t  getImageCount(VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int       initImageUsageFlags(VkImageUsageFlags *usageFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int       initSurfaceFormat(DeviceInfo device, VkSurfaceKHR surface, VkSurfaceFormatKHR *surfaceFormat);
    int       initPretransform(VkSurfaceTransformFlagBitsKHR *transformFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    int       initSwapchainImages(DeviceInfo device);
};


#endif //DYNAMICLINK_SWAPCHAIN_H
