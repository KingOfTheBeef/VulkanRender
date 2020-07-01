//
// Created by jonat on 22/05/2020.
//

#include <iostream>
#include "Core.h"
const uint8_t layerCount = 2;
const char *layers[] = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_api_dump"};

#define graphicIndex 0
#define displayIndex 1
#define queueIndexMax 2

void Core::populateLayerInfo(VkInstanceCreateInfo &info) {
  // TODO: Add checks for layer availability
  info.enabledLayerCount = layerCount;
  info.ppEnabledLayerNames = layers;
}

void Core::init() {
  LoadVulkanLibrary(&this->vulkanLib);
  LoadVulkanSystemFunctions(this->vulkanLib);

  VkInstanceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  // TODO: Check here for enable layers...
  populateLayerInfo(info);

  uint32_t extensionCount = 0;
  VkExtensionProperties extensions[20];
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  extensionCount = extensionCount<20?extensionCount:20;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);

  // Code to enable extensions (to allow on-screen rendering)
  // Assuming windows here
  // TODO: Put this in a function, figure the best way to deal with arrays, use vector maybe?
  const uint32_t targetExtensionCount = 2;
  const char* targetExtensions[targetExtensionCount] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
  for (int i = 0; i < targetExtensionCount; i++) {
    bool isPresent = false;
    for (int j = 0; j < extensionCount; j++) {
      if (strcmp(targetExtensions[i], extensions[j].extensionName) == 0) {
        isPresent = true;
        break;
      }
    }
    if (!isPresent) {
      std::cout << "Unable to find required extension: " << targetExtensions[i] << std::endl;
      return;
    } else {
      std::cout << "Found required extension: " << targetExtensions[i] << std::endl;
    }
  }

  info.enabledExtensionCount = targetExtensionCount;
  info.ppEnabledExtensionNames = targetExtensions;

  // Make instance
  if (vkCreateInstance(&info, nullptr, &this->instance) != VK_SUCCESS) {
    std::cout << "Failure to create instance" << std::endl;
  }

  LoadVulkanInstanceFunctions(this->instance);

  initWindowContext();
  initSurface();
  initDevice();
  initSwapchain();
}

void Core::clean() {
  if (this->swapchain != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(this->device.logical, this->swapchain, nullptr);
  }

  if (this->device.logical != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(this->device.logical);
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

Core::Core() : surface(VK_NULL_HANDLE), swapchain(VK_NULL_HANDLE) {
  this->vulkanLib = nullptr;
  this->instance = VK_NULL_HANDLE;
  this->device = {VK_NULL_HANDLE, VK_NULL_HANDLE};
}


/* TODO: This function too long. Future problem: only supports using a single device.
   This will require changes to the way we import device-level functions. */
void Core::initDevice() {
  uint32_t deviceCount;
  vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    std::cout << "No devices detected!" << std::endl;
    return;
  }
  // Select a physical device
  VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[deviceCount];
  vkEnumeratePhysicalDevices(this->instance, &deviceCount, physicalDevices);
  uint32_t queueIndices[queueIndexMax];
  const uint8_t targetExtensionCount = 1;
  const char* targetExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  for (uint32_t i = 0; i < deviceCount; i++) {
    if (validatePhysicalDevice(physicalDevices[i], queueIndices, targetExtensionCount, targetExtensions)) {
      this->device.physical = physicalDevices[i];
      break;
    }
  }

  if (this->device.physical == VK_NULL_HANDLE) {
    std::cout << "Failed to find a physical handle" << std::endl;
  }

  uint8_t queuesUsedCount = 0;
  const float queuePriorities = 1.0f;
  VkDeviceQueueCreateInfo queueCreateInfo[queueIndexMax];

  queuesUsedCount++;
  queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo[0].pNext = nullptr;
  queueCreateInfo[0].flags = 0;
  queueCreateInfo[0].queueFamilyIndex = queueIndices[graphicIndex];
  queueCreateInfo[0].queueCount = 1;
  queueCreateInfo[0].pQueuePriorities = &queuePriorities;
  if (queueIndices[graphicIndex] != queueIndices[displayIndex]) { // TODO: This is kinda filthy
    queuesUsedCount++;
    queueCreateInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[1].pNext = nullptr;
    queueCreateInfo[1].flags = 0;
    queueCreateInfo[1].queueFamilyIndex = queueIndices[displayIndex];
    queueCreateInfo[1].queueCount = 1;
    queueCreateInfo[1].pQueuePriorities = &queuePriorities;
  }

  VkDeviceCreateInfo deviceInfo = {};
  deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceInfo.flags = 0;
  deviceInfo.pNext = nullptr;
  deviceInfo.enabledLayerCount = 0;
  deviceInfo.ppEnabledLayerNames = nullptr;
  deviceInfo.queueCreateInfoCount = queuesUsedCount;
  deviceInfo.pQueueCreateInfos = queueCreateInfo;
  deviceInfo.enabledExtensionCount = 1;
  deviceInfo.ppEnabledExtensionNames = &targetExtensions[0];
  deviceInfo.pEnabledFeatures = nullptr;


  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(this->device.physical, &properties);
  std::cout << "Device Selected: " << properties.deviceName << std::endl;
  if (vkCreateDevice(this->device.physical, &deviceInfo, nullptr, &this->device.logical) != VK_SUCCESS) {
    std::cout << "Failed to create logical device" << std::endl;
  }

  this->device.graphicQueueIndex = queueIndices[graphicIndex];
  this->device.displayQueueIndex = queueIndices[displayIndex];
  LoadVulkanDeviceFunctions(this->device.logical);
  vkGetDeviceQueue(this->device.logical, this->device.graphicQueueIndex, 0, &this->device.graphicQueue);
  vkGetDeviceQueue(this->device.logical, this->device.displayQueueIndex, 0, &this->device.displayQueue);
  delete[](physicalDevices);
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
  this->windowContext->initialise("Dynamic Link", 500, 500);
}

bool Core::validatePhysicalDevice(VkPhysicalDevice physicalDevice, uint32_t *queueIndices, const uint8_t targetExtensionCount,
                                  const char **targetExtensions) {
  // Check for queue
  uint32_t queueFamilyCount = 0;
  VkQueueFamilyProperties queueFamilyProperties[20];
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
  queueFamilyCount = queueFamilyCount<20?queueFamilyCount:20;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);

  VkBool32 surfaceSupport;
  //TODO: Forgive me for this terrible code, what am I doing with these bitwise :(
  uint8_t queueFoundBitmap = 0b11;
  for (int i = 0; i < queueFamilyCount; i++) {
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, this->surface, &surfaceSupport);
    if (queueFamilyProperties[i].queueCount > 0
        && queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      if (queueFoundBitmap & 0b1) {
        queueIndices[graphicIndex] = i;
        queueFoundBitmap ^= 0b1;
      }
      if (surfaceSupport == VK_TRUE) {
        queueIndices[displayIndex] = i;
        queueFoundBitmap = 0b00;
        break;
      }
    } else if (surfaceSupport == VK_TRUE && queueFoundBitmap & 0b10) {
      queueIndices[displayIndex] = i;
      queueFoundBitmap ^= 0b10;
    }
  }

  if ((queueFoundBitmap ^ 0b11) != 0b11) {
    std::cout << "Failed to find queues to be satisfactory." << std::endl;
    return false;
  }

  // Check for swap chain extension
  uint32_t extensionCount = 0;
  VkExtensionProperties extensionProperties[20];
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
  extensionCount = extensionCount<20?extensionCount:20;
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
      std::cout << "Failed to find device extension " << targetExtensions[i] << std::endl;
      return false;
    }
  }

  return true;
}


// TODO: Need to refactor this function
void Core::initSwapchain() {
  uint32_t presentModeCount = 0;
  VkPresentModeKHR presentModes[20];
  vkGetPhysicalDeviceSurfacePresentModesKHR(this->device.physical, this->surface, &presentModeCount, nullptr);
  presentModeCount = presentModeCount<20?presentModeCount:20;
  vkGetPhysicalDeviceSurfacePresentModesKHR(this->device.physical, this->surface, &presentModeCount, presentModes);

  bool foundSuitablePresentMode = false;
  VkPresentModeKHR presentMode;
  for (uint32_t i = 0; i < presentModeCount; i++) {
    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
      foundSuitablePresentMode = true;
      break;
    } else if (presentModes[i] == VK_PRESENT_MODE_FIFO_KHR) {
      presentMode = VK_PRESENT_MODE_FIFO_KHR;
      foundSuitablePresentMode = true;
    }
  }

  if (!foundSuitablePresentMode) {
    std::cout << "Failed to find suitable present mode" << std::endl;
    return;
  }

  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device.physical, this->surface, &surfaceCapabilities);

  uint32_t imageCount =
          surfaceCapabilities.minImageCount==surfaceCapabilities.maxImageCount
          ?surfaceCapabilities.minImageCount + 1
          :surfaceCapabilities.minImageCount;

  VkExtent2D extent;
  getExtent2D(&extent, surfaceCapabilities);

  VkImageUsageFlags surfaceUsageFlags = 0;
  if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT
   && surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
    surfaceUsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  } else {
    std::cout << "Failed to get surface usage flags" << std::endl;
    return;
  }

  VkSurfaceTransformFlagBitsKHR transformFlags =
          surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
              ?VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
              :surfaceCapabilities.currentTransform;

  uint32_t surfaceFormatsCount = 0;
  VkSurfaceFormatKHR surfaceFormats[100];
  vkGetPhysicalDeviceSurfaceFormatsKHR(this->device.physical, this->surface, &surfaceFormatsCount, nullptr);
  std::cout << "Surface formats: " << (int)surfaceFormatsCount << std::endl;
  surfaceFormatsCount = surfaceFormatsCount<100? surfaceFormatsCount:100;
  vkGetPhysicalDeviceSurfaceFormatsKHR(this->device.physical, this->surface, &surfaceFormatsCount, surfaceFormats);
  VkSurfaceFormatKHR selectedSurfaceFormat = surfaceFormats[0];
  std::cout << selectedSurfaceFormat.format << std::endl;
  for (uint32_t i = 0; i < surfaceFormatsCount; i++) {
    if (surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM
    // && surfaceFormats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR
    ) {
      selectedSurfaceFormat = surfaceFormats[i];
      std::cout << "Found desired surface" << std::endl;
      break;
    }
  }

  VkSwapchainKHR prevSwapchain = this->swapchain;
  VkSwapchainCreateInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.pNext = nullptr;
  info.flags = 0;
  info.surface = this->surface;
  info.minImageCount = imageCount;
  info.imageFormat = selectedSurfaceFormat.format;
  info.imageColorSpace = selectedSurfaceFormat.colorSpace;
  info.imageExtent = extent;
  info.imageArrayLayers = 1;
  info.imageUsage = surfaceUsageFlags;
  info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.queueFamilyIndexCount = 0;
  info.pQueueFamilyIndices = nullptr;
  info.preTransform = transformFlags;
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

void Core::getExtent2D(VkExtent2D *extent, VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
  if (surfaceCapabilities.currentExtent.width == -1) {
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
    *extent = surfaceCapabilities.currentExtent;
  }
}

#undef graphicIndex
#undef displayIndex
#undef queueIndexMax
