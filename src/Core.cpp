//
// Created by jonat on 22/05/2020.
//

/* TODO: Deal with constants
 * Sometimes we pass value into functions to be initialised, others we get the return value. Perhaps make it consistent
 * Add code for checking if layers are supported (and if they are even enabled)
 * Maybe perhaps have it so that core is a memeber of window, (rather than the other way around> not sure if I want this)
 */
#include <iostream>
#include "Core.h"
#include "tempVertexData.h"

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
  // populateLayerInfo(&info);
  populateExtensionInfo(&info);
  // Make instance
  if (vkCreateInstance(&info, nullptr, &this->instance) != VK_SUCCESS) {
    std::cout << "Failure to create instance" << std::endl;
  }

  LoadVulkanInstanceFunctions(this->instance);
  initWindowContext();
  initSurface();
  initDevice();

  renderer.initRenderer(this->deviceInfo, this->surface);
}

void Core::clean() {
  if (this->deviceInfo.logical != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(this->deviceInfo.logical);

    this->renderer.clean(this->deviceInfo);

    vkDestroyDevice(this->deviceInfo.logical, nullptr);
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

Core::Core() : surface(VK_NULL_HANDLE) {
  this->vulkanLib = nullptr;
  this->instance = VK_NULL_HANDLE;
  this->deviceInfo = {VK_NULL_HANDLE, VK_NULL_HANDLE};
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
  populateQueueCreateInfo(&queueCreateInfo[0], this->deviceInfo.graphicQueueIndex, &queuePriorities);
  if (this->deviceInfo.graphicQueueIndex != this->deviceInfo.displayQueueIndex) { // This is kinda filthy
    queuesUsedCount++;
    populateQueueCreateInfo(&queueCreateInfo[1], this->deviceInfo.displayQueueIndex, &queuePriorities);
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
  vkGetPhysicalDeviceProperties(this->deviceInfo.physical, &properties);
  std::cout << "Device Selected: " << properties.deviceName
   << "\nMax vertex input bindings: " << properties.limits.maxVertexInputBindings
   << "\nMax DUBO: " << properties.limits.maxDescriptorSetUniformBuffersDynamic
   << "\nMin uniform buffer offset alignment: " << properties.limits.minUniformBufferOffsetAlignment << std::endl;
  if (vkCreateDevice(this->deviceInfo.physical, &deviceInfo, nullptr, &this->deviceInfo.logical) != VK_SUCCESS) {
    std::cout << "Failed to create logical device" << std::endl;
  }

  // Load vulkan device functions and get queue handle
  LoadVulkanDeviceFunctions(this->deviceInfo.logical);
  vkGetDeviceQueue(this->deviceInfo.logical, this->deviceInfo.graphicQueueIndex, 0, &this->deviceInfo.graphicQueue);
  vkGetDeviceQueue(this->deviceInfo.logical, this->deviceInfo.displayQueueIndex, 0, &this->deviceInfo.displayQueue);
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
  VkExtensionProperties extensionProperties[100];
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
  extensionCount = extensionCount < 100 ? extensionCount : 100;
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

void Core::draw() {
  switch (renderer.draw(this->deviceInfo)) {
    case VK_ERROR_OUT_OF_DATE_KHR:
      windowResize();
      break;
    default:
      break;
  }
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
      this->deviceInfo.physical = physicalDevices[i];
      break;
    }
  }
  this->deviceInfo.graphicQueueIndex = queueIndices[graphicIndex];
  this->deviceInfo.displayQueueIndex = queueIndices[displayIndex];
  delete(physicalDevices);

  if (this->deviceInfo.physical == VK_NULL_HANDLE) {
    return -1;
  }
  return 0;
}

void Core::windowResize() {
  this->windowContext->idleWhileMinimised();
  vkDeviceWaitIdle(this->deviceInfo.logical);
  renderer.windowResize(this->deviceInfo, this->surface);
}

void Core::update() {
  for (int i = 0; i < 8; i++) {
    Data::altVertexData[8 * i] += 0.1f;
  }
  // renderer.updateStagingBuffer(this->deviceInfo, Data::altVertexData, Data::vertexDataSize);
  // renderer.submitStagingBuffer(this->deviceInfo);
}

#undef graphicIndex
#undef displayIndex
#undef queueIndexMax
