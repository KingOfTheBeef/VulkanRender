//
// Created by jonat on 05/07/2020.
//

#ifndef DYNAMICLINK_RENDERER_H
#define DYNAMICLINK_RENDERER_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include "Structures.h"
#include "Swapchain.h"
#include "Buffer.h"
#include "DeviceMemory.h"


class Renderer {
public:
    Renderer();

    int initRenderer(DeviceInfo device, VkSurfaceKHR surface);

    int initResources(DeviceInfo device, const char *filename, CombinedImageSampler *texture);

    int initDescriptorSet(DeviceInfo device, DescriptorSet *descriptorSet);

    int updateTexture(DeviceInfo device, ImageFile imageFile, VkImage image);

    int windowResize(DeviceInfo device, VkSurfaceKHR surface);

    int draw(DeviceInfo device);

    void clean(DeviceInfo device);

private:
    Swapchain               swapchain;

    VkPipeline              pipeline;
    VkPipelineLayout        pipelineLayout;
    VkRenderPass            renderPass;
    VkCommandPool           cmdPool;
    static const uint32_t   virtualFrameCount = 3;
    uint32_t                currentVirtualFrame;
    VirtualFrame            virtualFrames[virtualFrameCount];

    Buffer                  vertexBuffer;
    Buffer                  indexBuffer;
    Buffer                  instanceBuffer;

    Buffer                  stagingBuffer;
    Buffer                  uniformBuffer;

    HostVisibleDeviceMemory hostVisibleMemory;
    DeviceMemory            deviceLocalMemory;

    static const uint32_t   descriptorSetCount = 1;
    DescriptorSet           descriptorSets[descriptorSetCount];

    CombinedImageSampler    texture;

private:
    int createImage(DeviceInfo device, uint32_t width, uint32_t height, VkImage *image);

    int createImageView(DeviceInfo device, VkImage image, VkImageView *imageView); // This function could also be used in core for swapchain images, (perhaps move swapchain into render class?)

    int initTexture(DeviceInfo device, uint32_t width, uint32_t height, Image *texture);

    int initRenderPass(VkDevice device, VkFormat format);

    int allocateImageMemory(DeviceInfo device, VkImage image, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceMemory *memory);

    int allocateMemory(DeviceInfo device, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceMemory *memory);

    int initShaderModule(VkDevice device, const char *filename, VkShaderModule *shaderModule);

    int initGraphicPipeline(DeviceInfo device);

    int initVirtualFrames(DeviceInfo device);

    int initCommandPool(DeviceInfo device);

    int prepareVirtualFrame(DeviceInfo device, VirtualFrame *virtualFrame, VkExtent2D extent, VkImageView *imageView, VkImage image);

    int initSampler(DeviceInfo device, VkSampler *sampler);

    int initDescriptorPool(DeviceInfo device, VkDescriptorPool *descriptorPool);

    int initDescriptorSetLayout(DeviceInfo device, VkDescriptorSetLayout *descriptorSetLayout);

    int allocateDescriptor(DeviceInfo device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout *descriptorLayout, VkDescriptorSet *descriptorSet);

    int updateDescriptor(DeviceInfo device, VkDescriptorSet descriptorSet, VkImageView imageView, VkSampler sampler, Buffer uniformBuffer, Buffer model);

    int updateStagingBuffer(DeviceInfo device, const void *data, size_t size);

    int submitStagingBuffer(DeviceInfo device, VkAccessFlagBits dstBufferAccessFlags, Buffer dstBuffer, uint64_t sizeOfData);
};

#endif //DYNAMICLINK_RENDERER_H
