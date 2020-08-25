//
// Created by jonat on 23/08/2020.
//

#ifndef DYNAMICLINK_DEVICEMEMORY_H
#define DYNAMICLINK_DEVICEMEMORY_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include "Structures.h"
#include "Buffer.h"

class HostVisibleDeviceMemory;
class DeviceMemory;

class DeviceMemory {
private:
    static void allocateBufferMemory(DeviceInfo device, int bufferCount, Buffer *buffers,
                                     VkMemoryPropertyFlags memoryPropertyFlags, DeviceMemory *deviceMemory);
public:
    // Create host visible memory
    static HostVisibleDeviceMemory createHostVisibleMemory(DeviceInfo device, int bufferCount, Buffer *buffers);
// Create generic memory
    static DeviceMemory createDeviceMemory(DeviceInfo device, int bufferCount, Buffer *buffers,
                                           VkMemoryPropertyFlags memoryPropertyFlags);

    DeviceMemory();
    void free(DeviceInfo device);

    VkDeviceMemory getHandle() { return this->handle; }

protected:
    virtual int allocateMemory(DeviceInfo device, VkMemoryRequirements memoryRequirements,
                                     VkMemoryPropertyFlags memoryPropertyFlags);

    int bindBuffers(DeviceInfo device, int bufferCount, Buffer *buffers);

protected:
    VkDeviceMemory          handle;
    VkDeviceSize            size;
    VkMemoryPropertyFlags   properties;
    uint32_t                type;
    // Maybe a list of all the buffers/images in this memory object

};

class HostVisibleDeviceMemory : public DeviceMemory {
public:
    HostVisibleDeviceMemory();
    void* getMappedMemory() { return this->mappedMemory; };
    void setMappedMemory(DeviceInfo device);
    void unmapMemory(DeviceInfo device);
    void free(DeviceInfo device);
private:
    int allocateMemory(DeviceInfo device, VkMemoryRequirements memoryRequirements,
                       VkMemoryPropertyFlags memoryPropertyFlags) override;

protected:
    void                    *mappedMemory;
};

#endif //DYNAMICLINK_DEVICEMEMORY_H
