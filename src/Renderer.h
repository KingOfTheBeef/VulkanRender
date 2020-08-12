//
// Created by jonat on 05/07/2020.
//

#ifndef DYNAMICLINK_RENDERER_H
#define DYNAMICLINK_RENDERER_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include "Structures.h"

class Renderer {
public:
    Renderer();

    int initRenderer(DeviceInfo device, VkFormat format);

    int initTextureResources(DeviceInfo device, const char *filename, CombinedImageSampler *texture);

    int initDescriptorSet(DeviceInfo device, DescriptorSet *descriptorSet);

    int updateTexture(DeviceInfo device, ImageFile imageFile, VkImage image);

    int updateStagingBuffer(DeviceInfo device, const void *data, size_t size);

    int submitStagingBuffer(DeviceInfo device);

    int draw(DeviceInfo device, SwapchainInfo swapchain);

    void clean(DeviceInfo device);

private:
    VkPipeline              pipeline;
    VkPipelineLayout        pipelineLayout;
    VkRenderPass            renderPass;
    VkCommandPool           cmdPool;
    static const uint32_t   virtualFrameCount = 3;
    uint32_t                currentVirtualFrame;
    VirtualFrame            virtualFrames[virtualFrameCount];
    VkBuffer                vertexBuffer;
    VkBuffer                stagingBuffer;
    VkDeviceMemory          hostVisibleMemory;
    VkDeviceMemory          deviceLocalMemory;

    static const uint32_t   descriptorSetCount = 1;
    DescriptorSet           descriptorSets[descriptorSetCount];

    CombinedImageSampler    texture;

private:
    int createImage(DeviceInfo device, uint32_t width, uint32_t height, VkImage *image);

    int createImageView(DeviceInfo device, VkImage image, VkImageView *imageView); // This function could also be used in core for swapchain images, (perhaps move swapchain into render class?)

    int initTexture(DeviceInfo device, uint32_t width, uint32_t height, Image *texture);

    int initRenderPass(VkDevice device, VkFormat format);

    int allocateBufferMemory(DeviceInfo device, VkBuffer buffer, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceMemory *memory);

    int allocateImageMemory(DeviceInfo device, VkImage image, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceMemory *memory);

    int allocateMemory(DeviceInfo device, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceMemory *memory);

    int initBuffer(DeviceInfo device, VkBufferUsageFlags bufferUsageFlags, size_t size, VkBuffer *buffer);

    int initShaderModule(VkDevice device, const char *filename, VkShaderModule *shaderModule);

    int initGraphicPipeline(DeviceInfo device);

    int initVirtualFrames(DeviceInfo device);

    int initCommandPool(DeviceInfo device);

    int initBuffersAndMemory(DeviceInfo device);

    int prepareVirtualFrame(DeviceInfo device, VirtualFrame *virtualFrame, VkExtent2D extent, VkImageView *imageView, VkImage image);

    int initSampler(DeviceInfo device, VkSampler *sampler);

    int initDescriptorPool(DeviceInfo device, VkDescriptorPool *descriptorPool);

    int initDescriptorSetLayout(DeviceInfo device, VkDescriptorSetLayout *descriptorSetLayout);

    int allocateDescriptor(DeviceInfo device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout *descriptorLayout, VkDescriptorSet *descriptorSet);

    int updateDescriptor(DeviceInfo device, VkDescriptorSet descriptorSet, VkImageView imageView, VkSampler sampler);
};

#endif //DYNAMICLINK_RENDERER_H
