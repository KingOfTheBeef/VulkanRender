//
// Created by jonat on 20/08/2020.
//

#include <iostream>
#include "Swapchain.h"
#include "VulkanStructures.h"

void Swapchain::initSwapchain(DeviceInfo device, VkSurfaceKHR surface) {
    VkPresentModeKHR presentMode;
    initPresentMode(device, surface, &presentMode);
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical, surface, &surfaceCapabilities);
    VkSurfaceFormatKHR surfaceFormat;
    initSurfaceFormat(device, surface, &surfaceFormat);

    VkSwapchainKHR prevSwapchain = this->handle;
    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.pNext = nullptr;
    info.flags = 0;
    info.surface = surface;
    info.minImageCount = getImageCount(surfaceCapabilities);
    info.imageFormat = surfaceFormat.format;
    info.imageColorSpace = surfaceFormat.colorSpace;
    initExtent2D(&this->extent, surfaceCapabilities);
    info.imageExtent = this->extent;
    info.imageArrayLayers = 1;
    if (initImageUsageFlags(&info.imageUsage, surfaceCapabilities)) {
        std::cout << "Failure to get usage flags" << std::endl;
    }
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    initPretransform(&info.preTransform, surfaceCapabilities);
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = presentMode;
    info.clipped = VK_TRUE;
    info.oldSwapchain = this->handle;

    if(vkCreateSwapchainKHR(device.logical, &info, nullptr, &this->handle) != VK_SUCCESS) {
        std::cout << "Error making swapchain" << std::endl;
    }
    this->surfaceFormat = surfaceFormat;

    if (prevSwapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device.logical, prevSwapchain, nullptr);
    }

    initSwapchainImages(device);
}

// Function finds the extent of the images/surface we use
void Swapchain::initExtent2D(VkExtent2D *extent, VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
        // TODO: Fix the use of 500, should be whatever the window dimensions are
        extent->width = extent->height = 500;
        if (extent-> width < surfaceCapabilities.minImageExtent.width) {
            extent->width = surfaceCapabilities.minImageExtent.width;
        } else if (extent->width > surfaceCapabilities.maxImageExtent.width) {
            extent->width = surfaceCapabilities.maxImageExtent.width;
        }
        if (extent->height < surfaceCapabilities.minImageExtent.height) {
            extent->height = surfaceCapabilities.minImageExtent.height;
        } else if (extent->height > surfaceCapabilities.maxImageExtent.height) {
            extent->height = surfaceCapabilities.maxImageExtent.height;
        }
    } else {
        // In this case, we have no choice but must use the extent provided
        *extent = surfaceCapabilities.currentExtent;
    }
}

// Function finds a suitable present mode
int Swapchain::initPresentMode(DeviceInfo device, VkSurfaceKHR surface, VkPresentModeKHR *presentMode) {
    uint32_t presentModeCount = 0;
    VkPresentModeKHR presentModes[20];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical, surface, &presentModeCount, nullptr);
    presentModeCount = presentModeCount<20?presentModeCount:20;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical, surface, &presentModeCount, presentModes);

    bool foundSuitablePresentMode = true;
    for (uint32_t i = 0; i < presentModeCount; i++) {
        // This is the most desirable present mode we want
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            *presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            foundSuitablePresentMode = false;
            break;
            // FIFO works too but is our second choice
        } else if (presentModes[i] == VK_PRESENT_MODE_FIFO_KHR) {
            *presentMode = VK_PRESENT_MODE_FIFO_KHR;
            foundSuitablePresentMode = false;
        }
    }

    return foundSuitablePresentMode;
}

// Function gets the number of images the swapchain will use.
uint32_t Swapchain::getImageCount(VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
    return  surfaceCapabilities.minImageCount==surfaceCapabilities.maxImageCount
            ?surfaceCapabilities.minImageCount + 1
            :surfaceCapabilities.minImageCount;
}

// Function to get flags for how we will use the images, We are looking for transfer and color attachment (render)
int Swapchain::initImageUsageFlags(VkImageUsageFlags *usageFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT
        && surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        *usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    } else {
        // Provided capabilities aren't sufficient
        return 1;
    }
    return 0;
}

// Function to select surface format
int Swapchain::initSurfaceFormat(DeviceInfo device, VkSurfaceKHR surface, VkSurfaceFormatKHR *surfaceFormat) {
    uint32_t surfaceFormatsCount = 0;
    VkSurfaceFormatKHR surfaceFormats[100];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical, surface, &surfaceFormatsCount, nullptr);
    surfaceFormatsCount = surfaceFormatsCount<100? surfaceFormatsCount:100;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical, surface, &surfaceFormatsCount, surfaceFormats);

    // Default to the first available format
    *surfaceFormat = surfaceFormats[0];
    for (uint32_t i = 0; i < surfaceFormatsCount; i++) {
        // Check if we can get a desirable format
        if (surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM
            // && surfaceFormats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR
                ) {
            *surfaceFormat = surfaceFormats[i];
            break;
        }
    }
    return 0;
}

// Select pretransform, this is useful when rendering to things like tablets which can change orientation
int Swapchain::initPretransform(VkSurfaceTransformFlagBitsKHR *transformFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
    *transformFlags =
            surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
            ?VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
            :surfaceCapabilities.currentTransform;
    return 0;
}

int Swapchain::initSwapchainImages(DeviceInfo device) {
    vkGetSwapchainImagesKHR(device.logical, this->handle, &this->imageCount, nullptr);
    if (this->imageCount == 0) {
        std::cout << "WARNING: Swapchain initialised with 0 images!" << std::endl;
    }

    this->images = new VkImage[this->imageCount];
    this->imageViews = new VkImageView[this->imageCount];
    vkGetSwapchainImagesKHR(device.logical, this->handle, &this->imageCount, this->images);

    for (int i = 0; i < this->imageCount; i++) {
        // Create the image view for that image
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.image = this->images[i];
        createInfo.format = this->surfaceFormat.format;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseMipLevel = 0;

        vkCreateImageView(device.logical, &createInfo, nullptr, &this->imageViews[i]);
    }

    return 0;
}

void Swapchain::clean(DeviceInfo device) {
    for (int i = 0; i < this->imageCount; i++) {
        vkDestroyImageView(device.logical, this->imageViews[i], nullptr);
    }
    if (this->handle != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device.logical, this->handle, nullptr);
        delete(this->images);
    }
}

void Swapchain::recreateSwapchain(DeviceInfo device, VkSurfaceKHR surface) {
    if (this->handle != VK_NULL_HANDLE) {
        for (int i = 0; i < this->imageCount; i++) {
            vkDestroyImageView(device.logical, this->imageViews[i], nullptr);
        }
    }

    if (this->handle != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device.logical, this->handle, nullptr);
        delete(this->images);
        this->handle = VK_NULL_HANDLE;
        this->imageCount = 0;
    }
    initSwapchain(device, surface);
}

Swapchain::Swapchain() {
    this->handle = VK_NULL_HANDLE;
    this->surfaceFormat = {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    this->extent = {0,0};
    this->imageCount = 0;
    this->images = nullptr;
    this->imageViews = nullptr;
}

VkResult Swapchain::acquireImage(DeviceInfo device, uint32_t *imageIndex, VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE) {
    return vkAcquireNextImageKHR(device.logical, this->getHandle(), UINT64_MAX, imageAvailableSemaphore,
            VK_NULL_HANDLE, imageIndex);
}

VkResult Swapchain::presentImage(DeviceInfo device, uint32_t imageIndex, VkSemaphore imageFinishProcessingSemaphore = VK_NULL_HANDLE) {
    VkPresentInfoKHR presentInfo = VKSTRUCT::presentInfoKhr(1, &this->handle, &imageIndex,
            (imageFinishProcessingSemaphore == VK_NULL_HANDLE)?0:1, &imageFinishProcessingSemaphore);

    return vkQueuePresentKHR(device.displayQueue, &presentInfo);
}
