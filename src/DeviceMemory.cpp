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

void DeviceMemory::free(DeviceInfo device) {
    vkFreeMemory(device.logical, this->handle, nullptr);
    this->handle = VK_NULL_HANDLE;
    this->size = 0;
    this->properties = 0;
    this->type = -1;
}

DeviceMemory::DeviceMemory() : handle(VK_NULL_HANDLE) {
    this->size = 0;
    this->properties = 0;
    this->type = -1;
}

HostVisibleDeviceMemory
DeviceMemory::allocateHostVisibleBufferMemory(DeviceInfo device, int bufferCount, Buffer *buffers) {
    HostVisibleDeviceMemory deviceMemory;
    DeviceMemory::allocBuffMem(device, bufferCount, buffers, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &deviceMemory);
    deviceMemory.setMappedMemory(device);
    return deviceMemory;
}

void DeviceMemory::allocBuffMem(DeviceInfo device, int bufferCount, Buffer *buffers,
                                VkMemoryPropertyFlags memoryPropertyFlags, DeviceMemory *deviceMemory) {
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

    deviceMemory->allocateMemory(device, memoryRequirements, memoryPropertyFlags);
    deviceMemory->bindBuffers(device, bufferCount, buffers);
}

void HostVisibleDeviceMemory::setMappedMemory(DeviceInfo device) {
    if (this->mappedMemory == nullptr) {
        vkMapMemory(device.logical, this->handle, 0, VK_WHOLE_SIZE, 0, &this->mappedMemory);
    }
}

void HostVisibleDeviceMemory::unmapMemory(DeviceInfo device) {
    if (this->mappedMemory != nullptr) {
        vkUnmapMemory(device.logical, this->handle);
        this->mappedMemory = nullptr;
    }
}

int HostVisibleDeviceMemory::allocateMemory(DeviceInfo device, VkMemoryRequirements memoryRequirements,
                                            VkMemoryPropertyFlags memoryPropertyFlags) {
    this->DeviceMemory::allocateMemory(device, memoryRequirements, memoryPropertyFlags);
    this->setMappedMemory(device);
    return 0;
}

HostVisibleDeviceMemory::HostVisibleDeviceMemory() {
    this->DeviceMemory::DeviceMemory();
    this->mappedMemory = nullptr;
}
