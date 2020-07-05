//
// Created by jonat on 22/05/2020.
//

// TODO:Need to sort out magic numbers

#include <iostream>
#include "Core.h"

const uint8_t layerCount = 2;
const char *layers[] = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_api_dump"};
const uint32_t targetExtensionCount = 2;
const char* targetExtensions[targetExtensionCount] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
const uint32_t targetDeviceExtensionsCount = 1;
const char* targetDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};


#define graphicIndex 0
#define displayIndex 1
#define queueIndexMax 2

void Core::populateLayerInfo(VkInstanceCreateInfo *info) {
  // TODO: Add checks for layer availability
  info->enabledLayerCount = layerCount;
  info->ppEnabledLayerNames = layers;
}

int Core::populateExtensionInfo(VkInstanceCreateInfo *info) {
  uint32_t extensionCount = 0;
  VkExtensionProperties extensions[20];
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  extensionCount = extensionCount<20?extensionCount:20;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);

  // Code to enable extensions (to allow on-screen rendering)
  // Assuming windows here
  for (int i = 0; i < targetExtensionCount; i++) {
    bool isPresent = false;
    for (int j = 0; j < extensionCount; j++) {
      if (strcmp(targetExtensions[i], extensions[j].extensionName) == 0) {
        isPresent = true;
        break;
      }
    }
    if (!isPresent) {
      return false;
    } else {
      std::cout << "Found required extension: " << targetExtensions[i] << std::endl;
    }
  }

  info->enabledExtensionCount = targetExtensionCount;
  info->ppEnabledExtensionNames = targetExtensions;
  return true;
}

void Core::init() {
  LoadVulkanLibrary(&this->vulkanLib);
  LoadVulkanSystemFunctions(this->vulkanLib);

  VkInstanceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  // TODO: Check here for if we want to enable layers, (debug purposes)...
  populateLayerInfo(&info);
  populateExtensionInfo(&info);
  // Make instance
  if (vkCreateInstance(&info, nullptr, &this->instance) != VK_SUCCESS) {
    std::cout << "Failure to create instance" << std::endl;
  }

  LoadVulkanInstanceFunctions(this->instance);
  initWindowContext();
  initSurface();
  initDevice();
  initSwapchain();
  initSemaphores();
  initCommandBuffers();
  recordCommandBuffers();
}

void Core::clean() {
  if (this->device.logical != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(this->device.logical);

    if (this->cmdBufferCount > 0) {
      vkFreeCommandBuffers(this->device.logical, this->cmdPool, this->cmdBufferCount, this->cmdBuffers);
      this->cmdBufferCount = 0;
      delete(this->cmdBuffers);
    }

    if (this->cmdPool != VK_NULL_HANDLE) {
      vkDestroyCommandPool(this->device.logical, this->cmdPool, nullptr);
      this->cmdPool = VK_NULL_HANDLE;
    }

    if (this->imageFinishProcessingSema != VK_NULL_HANDLE) {
      vkDestroySemaphore(this->device.logical, this->imageFinishProcessingSema, nullptr);
    }

    if (this->imageAvailableSema != VK_NULL_HANDLE) {
      vkDestroySemaphore(this->device.logical, this->imageAvailableSema, nullptr);
    }

    if (this->swapchain != VK_NULL_HANDLE) {
      vkDestroySwapchainKHR(this->device.logical, this->swapchain, nullptr);
    }

    vkDestroyDevice(this->device.logical, nullptr);
  }

  if (this->surface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
  }

  this->windowContext->clean();

  if (this->instance != VK_NULL_HANDLE) {
    vkDestroyInstance(this->instance, nullptr);
  }

  if (vulkanLib != nullptr) {
    FreeLibrary(vulkanLib);
  }
}

Core::Core() : surface(VK_NULL_HANDLE), swapchain(VK_NULL_HANDLE),
imageAvailableSema(VK_NULL_HANDLE), imageFinishProcessingSema(VK_NULL_HANDLE) {
  this->vulkanLib = nullptr;
  this->instance = VK_NULL_HANDLE;
  this->device = {VK_NULL_HANDLE, VK_NULL_HANDLE};
}


/* TODO: This function too long. Future problem: only supports using a single device.
   This will require changes to the way we import device-level functions. */
void Core::initDevice() {
  selectPhysicalDevice();

  // Populate queue create infos
  uint8_t queuesUsedCount = 0;
  const float queuePriorities = 1.0f;
  VkDeviceQueueCreateInfo queueCreateInfo[queueIndexMax];
  queuesUsedCount++;
  populateQueueCreateInfo(&queueCreateInfo[0], this->device.graphicQueueIndex, &queuePriorities);
  if (this->device.graphicQueueIndex != this->device.displayQueueIndex) { // This is kinda filthy
    queuesUsedCount++;
    populateQueueCreateInfo(&queueCreateInfo[1], this->device.displayQueueIndex, &queuePriorities);
  }

  // Populate device create infos
  VkDeviceCreateInfo deviceInfo = {};
  deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceInfo.flags = 0;
  deviceInfo.pNext = nullptr;
  deviceInfo.enabledLayerCount = 0;
  deviceInfo.ppEnabledLayerNames = nullptr;
  deviceInfo.queueCreateInfoCount = queuesUsedCount;
  deviceInfo.pQueueCreateInfos = queueCreateInfo;
  deviceInfo.enabledExtensionCount = targetDeviceExtensionsCount;
  deviceInfo.ppEnabledExtensionNames = &targetDeviceExtensions;
  deviceInfo.pEnabledFeatures = nullptr;

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(this->device.physical, &properties);
  std::cout << "Device Selected: " << properties.deviceName << std::endl;
  if (vkCreateDevice(this->device.physical, &deviceInfo, nullptr, &this->device.logical) != VK_SUCCESS) {
    std::cout << "Failed to create logical device" << std::endl;
  }

  // Load vulkan device functions and get queue handles
  LoadVulkanDeviceFunctions(this->device.logical);
  vkGetDeviceQueue(this->device.logical, this->device.graphicQueueIndex, 0, &this->device.graphicQueue);
  vkGetDeviceQueue(this->device.logical, this->device.displayQueueIndex, 0, &this->device.displayQueue);
}

void Core::populateQueueCreateInfo(VkDeviceQueueCreateInfo *queueCreateInfo, uint32_t queueFamilyIndex, const float *queuePriorities) {
  queueCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo->pNext = nullptr;
  queueCreateInfo->flags = 0;
  queueCreateInfo->queueFamilyIndex = queueFamilyIndex;
  queueCreateInfo->queueCount = 1;
  queueCreateInfo->pQueuePriorities = queuePriorities;
}

void Core::initSurface() {
  VkWin32SurfaceCreateInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  info.pNext = nullptr;
  info.flags = 0;
  info.hinstance = windowContext->getWindowInstance();
  info.hwnd = windowContext->getWindowHWND();

  if (vkCreateWin32SurfaceKHR(this->instance, &info, nullptr, &this->surface) != VK_SUCCESS) {
    std::cout << "Failed to create surface" << std::endl;
  }
}

void Core::initWindowContext() {
  this->windowContext = new WindowContext();
  this->windowContext->initialise("Dynamic Link", 600, 600);
}

// Validate and get the device queues that we need, the queue family indices will be stored in the queueIndices array
bool Core::checkPhysicalDeviceQueues(VkPhysicalDevice physicalDevice, uint32_t *queueIndices) {
  uint32_t queueFamilyCount = 0;
  VkQueueFamilyProperties queueFamilyProperties[20];
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
  queueFamilyCount = queueFamilyCount<20?queueFamilyCount:20;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);
  VkBool32 surfaceSupport;

  // The code below is pretty messy but what it aims to do is to find if we have queues which provide surface support
  // and/or graphic operations support (since you can do off screen rendering), we prefer a setup where they are the same
  // family
  uint8_t queueFoundBitmap = 0b11;
  for (int i = 0; i < queueFamilyCount; i++) {
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, this->surface, &surfaceSupport);
    if (queueFamilyProperties[i].queueCount > 0) {
      if (queueFoundBitmap & 0b1 && queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        queueIndices[graphicIndex] = i;
        queueFoundBitmap ^= 0b1;
        // We check here again becaause we prefer it when we can have both the graphic and display as the same family
        if (surfaceSupport == VK_TRUE) {
          queueIndices[displayIndex] = i;
          queueFoundBitmap = 0b00;
          break;
        }
      }
      if (queueFoundBitmap & 0b10 && surfaceSupport == VK_TRUE) {
        queueIndices[displayIndex] = i;
        queueFoundBitmap ^= 0b10;
      }
    }
  }

  return (queueFoundBitmap ^ 0b11) == 0b11;
}

bool Core::checkPhysicalDeviceExtensions(VkPhysicalDevice physicalDevice, const uint8_t targetExtensionCount, const char **targetExtensions) {
  // Check for device extensions (e.g. swap chain)
  uint32_t extensionCount = 0;
  VkExtensionProperties extensionProperties[20];
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
  extensionCount = extensionCount < 20 ? extensionCount : 20;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProperties);

  for (uint8_t i = 0; i < targetExtensionCount; i++) {
    bool extensionFound = false;
    for (uint8_t j = 0; j < extensionCount; j++) {
      if (strcmp(targetExtensions[i], extensionProperties[j].extensionName) == 0) {
        extensionFound = true;
        break;
      }
    }
    if (!extensionFound) {
      return false;
    }
  }
  return true;
}

void Core::initSwapchain() {
  VkPresentModeKHR presentMode;
  initPresentMode(&presentMode);
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device.physical, this->surface, &surfaceCapabilities);
  VkSurfaceFormatKHR surfaceFormat;
  initSurfaceFormat(&surfaceFormat);

  VkSwapchainKHR prevSwapchain = this->swapchain;
  VkSwapchainCreateInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.pNext = nullptr;
  info.flags = 0;
  info.surface = this->surface;
  info.minImageCount = getImageCount(surfaceCapabilities);
  info.imageFormat = surfaceFormat.format;
  info.imageColorSpace = surfaceFormat.colorSpace;
  initExtent2D(&info.imageExtent, surfaceCapabilities);
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
  info.oldSwapchain = this->swapchain;

  if(vkCreateSwapchainKHR(this->device.logical, &info, nullptr, &this->swapchain) != VK_SUCCESS) {
    std::cout << "Error making swapchain" << std::endl;
  }

  if (prevSwapchain != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(this->device.logical, prevSwapchain, nullptr);
  }
}

// Function finds the extent of the images/surface we use
void Core::initExtent2D(VkExtent2D *extent, VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
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
int Core::initPresentMode(VkPresentModeKHR *presentMode) {
  uint32_t presentModeCount = 0;
  VkPresentModeKHR presentModes[20];
  vkGetPhysicalDeviceSurfacePresentModesKHR(this->device.physical, this->surface, &presentModeCount, nullptr);
  presentModeCount = presentModeCount<20?presentModeCount:20;
  vkGetPhysicalDeviceSurfacePresentModesKHR(this->device.physical, this->surface, &presentModeCount, presentModes);

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
uint32_t Core::getImageCount(VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
  return  surfaceCapabilities.minImageCount==surfaceCapabilities.maxImageCount
          ?surfaceCapabilities.minImageCount + 1
          :surfaceCapabilities.minImageCount;
}

// Function to get flags for how we will use the images, We are looking for transfer and color attachment (render)
int Core::initImageUsageFlags(VkImageUsageFlags *usageFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
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
int Core::initSurfaceFormat(VkSurfaceFormatKHR *surfaceFormat) {
  uint32_t surfaceFormatsCount = 0;
  VkSurfaceFormatKHR surfaceFormats[100];
  vkGetPhysicalDeviceSurfaceFormatsKHR(this->device.physical, this->surface, &surfaceFormatsCount, nullptr);
  surfaceFormatsCount = surfaceFormatsCount<100? surfaceFormatsCount:100;
  vkGetPhysicalDeviceSurfaceFormatsKHR(this->device.physical, this->surface, &surfaceFormatsCount, surfaceFormats);

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
int Core::initPretransform(VkSurfaceTransformFlagBitsKHR *transformFlags, VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
  *transformFlags =
          surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
          ?VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
          :surfaceCapabilities.currentTransform;
  return 0;
}

int Core::initCommandBuffers() {
  VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
  cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmdPoolCreateInfo.pNext = nullptr;
  cmdPoolCreateInfo.flags = 0;
  cmdPoolCreateInfo.queueFamilyIndex = this->device.displayQueueIndex;

  if (vkCreateCommandPool(this->device.logical, &cmdPoolCreateInfo, nullptr, &this->cmdPool) != VK_SUCCESS) {
    std::cout << "Failure to create command pool" << std::endl;
  }

  this->cmdBufferCount = 0;
  vkGetSwapchainImagesKHR(this->device.logical, this->swapchain, &this->cmdBufferCount, nullptr);
  if ( this->cmdBufferCount == 0 ) {
    std::cout << "Failure to get swapchain images" << std::endl;
    return -1;
  }

  this->cmdBuffers = new VkCommandBuffer[this->cmdBufferCount];

  VkCommandBufferAllocateInfo allocateInfo = {};
  allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.pNext = nullptr;
  allocateInfo.commandPool = this->cmdPool;
  allocateInfo.commandBufferCount = this->cmdBufferCount;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  if (vkAllocateCommandBuffers(this->device.logical, &allocateInfo, this->cmdBuffers) != VK_SUCCESS) {
    std::cout << "Failed to allocate command buffers" << std::endl;
    return -1;
  }

  return 0;
}

int Core::recordCommandBuffers() {
  // Think about making images a class property
  VkImage *images = new VkImage[this->cmdBufferCount];
  vkGetSwapchainImagesKHR(this->device.logical, this->swapchain, &this->cmdBufferCount, images);

  VkCommandBufferBeginInfo commandBufferBeginInfo = {};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  VkClearColorValue color = {{1.0f, 0.4f, 0.4f, 0.0f}};

  VkImageSubresourceRange subresourceRange = {};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;

  for (int i = 0; i < this->cmdBufferCount; i++) {
    VkImageMemoryBarrier preClearMemBarrier = {};
    preClearMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    preClearMemBarrier.pNext = nullptr;
    preClearMemBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    preClearMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    preClearMemBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    preClearMemBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    preClearMemBarrier.srcQueueFamilyIndex = this->device.displayQueueIndex;
    preClearMemBarrier.dstQueueFamilyIndex = this->device.displayQueueIndex;
    preClearMemBarrier.image = images[i];
    preClearMemBarrier.subresourceRange = subresourceRange;

    VkImageMemoryBarrier postClearMemBarrier = {};
    postClearMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    postClearMemBarrier.pNext = nullptr;
    postClearMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    postClearMemBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    postClearMemBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    postClearMemBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    postClearMemBarrier.srcQueueFamilyIndex = this->device.displayQueueIndex;
    postClearMemBarrier.dstQueueFamilyIndex = this->device.displayQueueIndex;
    postClearMemBarrier.image = images[i];
    postClearMemBarrier.subresourceRange = subresourceRange;


    vkBeginCommandBuffer(this->cmdBuffers[i], &commandBufferBeginInfo);
    vkCmdPipelineBarrier(this->cmdBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &preClearMemBarrier);
    vkCmdClearColorImage(this->cmdBuffers[i], images[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &subresourceRange);
    vkCmdPipelineBarrier(this->cmdBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &postClearMemBarrier);
    if (vkEndCommandBuffer(this->cmdBuffers[i]) != VK_SUCCESS) {
      std::cout << "Failure to record cmd buffer" << std::endl;
      delete(images);
      return -1;
    }
  }

  delete(images);
  return 0;
}

void Core::draw() {
  uint32_t imageIndex = 0;
  vkAcquireNextImageKHR(this->device.logical, this->swapchain, UINT64_MAX, this->imageAvailableSema, VK_NULL_HANDLE, &imageIndex);

  VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.pNext = nullptr;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &this->cmdBuffers[imageIndex];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &this->imageFinishProcessingSema;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &this->imageAvailableSema;
  submitInfo.pWaitDstStageMask = &stageFlags;

  vkQueueSubmit(this->device.displayQueue, 1, &submitInfo, VK_NULL_HANDLE);

  VkResult result;
  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pNext = nullptr;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &this->swapchain;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &this->imageFinishProcessingSema;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = &result;

  vkQueuePresentKHR(this->device.displayQueue, &presentInfo);

  if (result != VK_SUCCESS) {
    std::cout << "Something fishy" << std::endl;
  }
}

int Core::initSemaphores() {
  VkSemaphoreCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  if (vkCreateSemaphore(this->device.logical, &createInfo, nullptr, &this->imageAvailableSema) != VK_SUCCESS) {
    std::cout << "Failed to make semaphore imageAvailableSema" << std::endl;
  }
  if (vkCreateSemaphore(this->device.logical, &createInfo, nullptr, &this->imageFinishProcessingSema) != VK_SUCCESS) {
    std::cout << "Failed to make semaphore imageFinishProcessingSema" << std::endl;
  }
  return 0;
}

// Function which sets the physical device and queue indexes
int Core::selectPhysicalDevice() {
  uint32_t deviceCount;
  vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    std::cout << "No devices detected!" << std::endl;
    return -1;
  }
  // Select a physical device
  VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[deviceCount];
  vkEnumeratePhysicalDevices(this->instance, &deviceCount, physicalDevices);
  uint32_t queueIndices[queueIndexMax];
  for (uint32_t i = 0; i < deviceCount; i++) {
    if (checkPhysicalDeviceQueues(physicalDevices[i], queueIndices)
        && checkPhysicalDeviceExtensions(physicalDevices[i], targetDeviceExtensionsCount, &targetDeviceExtensions)) {
      this->device.physical = physicalDevices[i];
      std::cout << this->device.physical << std::endl;
      break;
    }
  }
  this->device.graphicQueueIndex = queueIndices[graphicIndex];
  this->device.displayQueueIndex = queueIndices[displayIndex];
  delete(physicalDevices);

  if (this->device.physical == VK_NULL_HANDLE) {
    return -1;
  }
  return 0;
}

#undef graphicIndex
#undef displayIndex
#undef queueIndexMax
