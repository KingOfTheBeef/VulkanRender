//
// Created by jonat on 22/05/2020.
//

#ifndef DYNAMICLINK_CORE_H
#define DYNAMICLINK_CORE_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <time.h>

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

    Renderer renderer;

    clock_t lastDrawCall = clock();
    float ticks_per_frame = CLOCKS_PER_SEC / 60.0f;

public:
    Core();

    void      init();
    void      draw();
    void      clean();
    void      windowResize();
    void      update(void *data, size_t size);


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
};

#endif //DYNAMICLINK_CORE_H
