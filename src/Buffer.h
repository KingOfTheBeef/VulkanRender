//
// Created by jonat on 23/08/2020.
//

#ifndef DYNAMICLINK_BUFFER_H
#define DYNAMICLINK_BUFFER_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include "Structures.h"

class Buffer {
public:
    static Buffer createBuffer(DeviceInfo device, VkBufferUsageFlags bufferUsageFlags, size_t size);

    void destroy(DeviceInfo device);

    VkBuffer    getHandle()                         { return this->handle; };
    void        setMemoryOffset(uint32_t offset)    { this->memoryOffset = offset; }
    uint32_t    getMemoryOffset()                   { return this->memoryOffset; }

private:
    VkBuffer        handle;
    VkDeviceSize    dataSize;
    uint32_t        memoryOffset;   // I don't think this property is necessary
};


#endif //DYNAMICLINK_BUFFER_H
