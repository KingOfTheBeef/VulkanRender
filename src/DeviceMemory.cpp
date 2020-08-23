//
// Created by jonat on 23/08/2020.
//

#include <iostream>
#include "DeviceMemory.h"

DeviceMemory DeviceMemory::allocateBufferMemory(DeviceInfo device, int bufferCount, Buffer *buffers,
                                   VkMemoryPropertyFlags memoryPropertyFlags) {
    DeviceMemory deviceMemory;

    VkMemoryRequirements memoryRequirements;
    memoryRequirements.size = 0;
    memoryRequirements.memoryTypeBits = -1;
    memoryRequirements.alignment = 0;
    for (int i = 0; i < bufferCount; i++) {
        VkMemoryRequirements currentRequirement;
        vkGetBufferMemoryRequirements(device.logical, buffers[i].getHandle(), &currentRequirement);

        // Add extra memory for the alignment (if needed)
        memoryRequirements.size +=
                (currentRequirement.alignment - memoryRequirements.size % currentRequirement.alignment) %
                currentRequirement.alignment;

        // Update buffers offset
        buffers[i].setMemoryOffset(memoryRequirements.size);

        memoryRequirements.size += + currentRequirement.size;
        memoryRequirements.memoryTypeBits &= currentRequirement.memoryTypeBits;
    }

    deviceMemory.allocateMemory(device, memoryRequirements, memoryPropertyFlags);

    deviceMemory.bindBuffers(device, bufferCount, buffers);

    return deviceMemory;
}

int DeviceMemory::allocateMemory(DeviceInfo device, VkMemoryRequirements memoryRequirements,
                             VkMemoryPropertyFlags memoryPropertyFlags) {

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(device.physical, &memoryProperties);

    uint32_t memoryIndex = -1;

    for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (memoryRequirements.memoryTypeBits & (1 << i)
            && (memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
            memoryIndex = i;
            break;
        }
    }
    if (memoryIndex == -1) {
        std::cout << "Didn't find memory for allocation" << std::endl;
        return -1;
    }

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = nullptr;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryIndex;

    vkAllocateMemory(device.logical, &memoryAllocateInfo, nullptr, &this->handle);

    this->type = memoryIndex;
    this->size = memoryRequirements.size;
    return 0;
}

int DeviceMemory::bindBuffers(DeviceInfo device, int bufferCount, Buffer *buffers) {
    for (int i = 0; i < bufferCount; i++) {
        vkBindBufferMemory(device.logical, buffers[i].getHandle(), this->handle, buffers[i].getMemoryOffset());
    }
    return 0;
}
