//
// Created by jonat on 23/08/2020.
//

#include "Buffer.h"

Buffer Buffer::createBuffer(DeviceInfo device, VkBufferUsageFlags bufferUsageFlags, size_t size) {
    Buffer thisBuffer;

    // Create Vulkan buffer
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.size = size;
    createInfo.usage = bufferUsageFlags;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;       // Only used when we have concurrent sharing mode
    createInfo.pQueueFamilyIndices = nullptr;
    vkCreateBuffer(device.logical, &createInfo, nullptr, &thisBuffer.handle);

    thisBuffer.dataSize = size;
    thisBuffer.memoryOffset = 0;

    return thisBuffer;
}

void Buffer::destroy(DeviceInfo device) {
    vkDestroyBuffer(device.logical, this->handle, nullptr);
    this->handle = VK_NULL_HANDLE;
    this->memoryOffset = 0;
    this->dataSize = 0;
}
