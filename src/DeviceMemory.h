//
// Created by jonat on 23/08/2020.
//

#ifndef DYNAMICLINK_DEVICEMEMORY_H
#define DYNAMICLINK_DEVICEMEMORY_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include "Structures.h"
#include "Buffer.h"

class DeviceMemory {
public:
    static DeviceMemory allocateBufferMemory(DeviceInfo device, int bufferCount, Buffer *buffers,
                                             VkMemoryPropertyFlags memoryPropertyFlags);

    VkDeviceMemory getHandle() { return this->handle; }

private:
    int allocateMemory(DeviceInfo device, VkMemoryRequirements memoryRequirements,
                                     VkMemoryPropertyFlags memoryPropertyFlags);

    int bindBuffers(DeviceInfo device, int bufferCount, Buffer *buffers);

private:
    VkDeviceMemory          handle;
    VkDeviceSize            size;
    VkMemoryPropertyFlags   properties;
    uint32_t                type;
    // Maybe a list of all the buffers/images in this memory object

};


#endif //DYNAMICLINK_DEVICEMEMORY_H
