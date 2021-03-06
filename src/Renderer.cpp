//
// Created by jonat on 05/07/2020.
//

#include <iostream>
#include "Renderer.h"
#include "FileReader.h"
#include "tempVertexData.h"
#include "GraphicMath.h"
#include "VulkanStructures.h"
#include "Buffer.h"
#include "DeviceMemory.h"

int Renderer::initRenderPass(VkDevice device, VkFormat format) {
    VkAttachmentDescription attachmentDescriptions = VKSTRUCT::attachmentDescription(format);
    VkAttachmentReference colorAttachmentReferences = VKSTRUCT::attachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkSubpassDescription subpassDescriptions = VKSTRUCT::subpassDescription(0, nullptr, 1, &colorAttachmentReferences);

    VkSubpassDependency subpassDependencies[] = {
            VKSTRUCT::subpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_DEPENDENCY_BY_REGION_BIT),
            VKSTRUCT::subpassDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                        VK_ACCESS_MEMORY_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT)
    };

    VkRenderPassCreateInfo createInfo = VKSTRUCT::renderPassCreateInfo(1, &attachmentDescriptions, 2, subpassDependencies, 1, &subpassDescriptions);
    if (vkCreateRenderPass(device, &createInfo, nullptr, &this->renderPass) != VK_SUCCESS) {
        std::cout << "Failed to create render pass" << std::endl;
        return -1;
    }
    return 0;
}

int Renderer::initShaderModule(VkDevice device, const char *filename, VkShaderModule *shaderModule) {
    BinaryFile prog;
    if (FileReader::loadFileBin(filename, &prog)) {
        std::cout << "Unable to open file " << filename << std::endl;
    }
    VkShaderModuleCreateInfo createInfo = VKSTRUCT::shaderModuleCreateInfo(prog.size, reinterpret_cast<uint32_t *>(prog.data));
    VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, shaderModule);
    FileReader::freeFileBin(&prog);
    return result != VK_SUCCESS;
}

void Renderer::clean(DeviceInfo device) {

    vkDestroyImageView(device.logical, this->texture.image.view, nullptr);
    vkDestroyImage(device.logical, this->texture.image.handle, nullptr);
    vkFreeMemory(device.logical, this->texture.image.memory, nullptr);

    vkDestroySampler(device.logical, this->texture.sampler, nullptr);

    vkDestroyDescriptorSetLayout(device.logical, this->descriptorSets[0].layout, nullptr);
    vkDestroyDescriptorPool(device.logical, this->descriptorSets[0].pool, nullptr);

    if (this->pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device.logical, this->pipelineLayout, nullptr);
    }

    for (auto &virtualFrame : this->virtualFrames) {
        vkFreeCommandBuffers(device.logical, this->cmdPool, 1, &virtualFrame.cmdBuffer);
        vkDestroySemaphore(device.logical, virtualFrame.imageFinishProcessingSema, nullptr);
        vkDestroySemaphore(device.logical, virtualFrame.imageAvailableSema, nullptr);
        vkDestroyFence(device.logical, virtualFrame.fence, nullptr);
        if (virtualFrame.framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(device.logical, virtualFrame.framebuffer, nullptr);
        }
    }

    this->stagingBuffer.destroy(device);
    this->vertexBuffer.destroy(device);
    this->instanceBuffer.destroy(device);
    this->indexBuffer.destroy(device);
    this->uniformBuffer.destroy(device);

    this->deviceLocalMemory.free(device);
    this->hostVisibleMemory.free(device);

    vkDestroyCommandPool(device.logical, this->cmdPool, nullptr);
    vkDestroyRenderPass(device.logical, this->renderPass, nullptr);
    vkDestroyPipeline(device.logical, this->pipeline, nullptr);

    this->swapchain.clean(device);
}

int Renderer::initGraphicPipeline(DeviceInfo device) {

    VkShaderModule vertexShader, fragmentShader;
    initShaderModule(device.logical, "shaders/vert.spv", &vertexShader);
    initShaderModule(device.logical, "shaders/frag.spv", &fragmentShader);
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[] = {
            VKSTRUCT::pipelineShaderStageCreateInfo(vertexShader, VK_SHADER_STAGE_VERTEX_BIT),
            VKSTRUCT::pipelineShaderStageCreateInfo(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT)
    };

    /* Vertex data arranged as =
     * {x, y, z, w, r, g, b, a}
     * i.e. 4 32bit floats for xyzw coordinates and then rgba color values, interleaved
     * */

    VkVertexInputBindingDescription vertexInputBindingDescription[2];
    vertexInputBindingDescription[0] = VKSTRUCT::vertexInputBindingDescription(0, sizeof(float) * 6, VK_VERTEX_INPUT_RATE_VERTEX);      // Vertex data
    vertexInputBindingDescription[1] = VKSTRUCT::vertexInputBindingDescription(1, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_INSTANCE);    // Instance Data

    VkVertexInputAttributeDescription vertexInputAttributeDescription[3];
    // Vertex
    vertexInputAttributeDescription[0] = VKSTRUCT::vertexInputAttributeDescription(0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, Data::positionOffset);
    vertexInputAttributeDescription[1] = VKSTRUCT::vertexInputAttributeDescription(0, VK_FORMAT_R32G32_SFLOAT, 1, Data::colorOffset);
    // Instance
    vertexInputAttributeDescription[2] = VKSTRUCT::vertexInputAttributeDescription(1, VK_FORMAT_R32G32_SFLOAT, 2, 0);

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = VKSTRUCT::pipelineVertexInputStateCreateInfo(2, vertexInputBindingDescription, 3, vertexInputAttributeDescription);
    VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo = VKSTRUCT::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = VKSTRUCT::pipelineViewportStateCreateInfo(1, 1);
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = VKSTRUCT::pipelineRasterizationStateCreateInfo();
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = VKSTRUCT::pipelineMultisampleStateCreateInfo();
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = VKSTRUCT::pipelineColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = VKSTRUCT::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachmentState);
    VkPipelineLayoutCreateInfo layoutCreateInfo = VKSTRUCT::pipelineLayoutCreateInfo(Renderer::descriptorSetCount, &this->descriptorSets[0].layout);
    vkCreatePipelineLayout(device.logical, &layoutCreateInfo, nullptr, &this->pipelineLayout);
    VkDynamicState dynamicStates[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = VKSTRUCT::pipelineDynamicStateCreateInfo(2, dynamicStates);
    VkGraphicsPipelineCreateInfo createInfo = VKSTRUCT::graphicsPipelineCreateInfo(
            this->renderPass, &vertexInputStateCreateInfo, &assemblyStateCreateInfo, &viewportStateCreateInfo,
            &rasterizationStateCreateInfo, &multisampleStateCreateInfo, &colorBlendStateCreateInfo,
            this->pipelineLayout, &dynamicStateCreateInfo, 2, shaderStageCreateInfo);

    if (vkCreateGraphicsPipelines(device.logical, 0, 1, &createInfo, nullptr, &this->pipeline) != VK_SUCCESS) {
        std::cout << "Failed to do so" << std::endl;
    }

    vkDestroyShaderModule(device.logical, vertexShader, nullptr);
    vkDestroyShaderModule(device.logical, fragmentShader, nullptr);
    return 0;
}

int
Renderer::prepareVirtualFrame(DeviceInfo device, VirtualFrame *virtualFrame, VkExtent2D extent, VkImageView *imageView,
                              VkImage image) {
    if (virtualFrame->framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(device.logical, virtualFrame->framebuffer, nullptr);
    }

    VkFramebufferCreateInfo framebufferCreateInfo = VKSTRUCT::framebufferCreateInfo(this->renderPass, 1, imageView, extent.width, extent.height);

    vkCreateFramebuffer(device.logical, &framebufferCreateInfo, nullptr, &virtualFrame->framebuffer);

    VkCommandBufferBeginInfo beginInfo = VKSTRUCT::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    vkBeginCommandBuffer(virtualFrame->cmdBuffer, &beginInfo);

    VkImageSubresourceRange subresourceRange = VKSTRUCT::imageSubresourceRange(1, 0, 1, 0, VK_IMAGE_ASPECT_COLOR_BIT);

    if (device.graphicQueueIndex != device.displayQueueIndex) {
        VkImageMemoryBarrier memoryBarrier = VKSTRUCT::imageMemoryBarrier(image, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, subresourceRange,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED, device.displayQueueIndex, device.graphicQueueIndex);

        vkCmdPipelineBarrier(virtualFrame->cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
    }

    VkClearValue clearColor = {1.0f, 0.8f, 0.4f, 0.0f};
    VkRenderPassBeginInfo renderPassBeginInfo = VKSTRUCT::renderPassBeginInfo(virtualFrame->framebuffer, this->renderPass, extent, 1, &clearColor);

    vkCmdBeginRenderPass(virtualFrame->cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(virtualFrame->cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);

    VkViewport viewport = VKSTRUCT::viewport(extent.width, extent.height, 0, 0, 0.0f, 1.0f);

    VkRect2D rect = VKSTRUCT::rect2D({0, 0}, extent);

    vkCmdSetViewport(virtualFrame->cmdBuffer, 0, 1, &viewport);
    vkCmdSetScissor(virtualFrame->cmdBuffer, 0, 1, &rect);

    VkDeviceSize offset = 0;
    VkBuffer buffers[2];
    buffers[0] = this->vertexBuffer.getHandle();
    buffers[1] = this->instanceBuffer.getHandle();

    vkCmdBindVertexBuffers(virtualFrame->cmdBuffer, 0, 1, &buffers[0], &offset);
    vkCmdBindVertexBuffers(virtualFrame->cmdBuffer, 1, 1, &buffers[1], &offset);

    vkCmdBindIndexBuffer(virtualFrame->cmdBuffer, this->indexBuffer.getHandle(), 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(virtualFrame->cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayout, 0,
                            this->descriptorSetCount, &this->descriptorSets[0].handle, 0, nullptr);

    // Data::
    // vkCmdDraw(virtualFrame->cmdBuffer, 6, 1, 0, 0);
    vkCmdDrawIndexed(virtualFrame->cmdBuffer, 6, 2, 0, 0, 0);

    vkCmdEndRenderPass(virtualFrame->cmdBuffer);

    if (device.graphicQueueIndex != device.displayQueueIndex) {
        VkImageMemoryBarrier memoryBarrier = VKSTRUCT::imageMemoryBarrier(
                image, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                subresourceRange, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED,
                device.graphicQueueIndex, device.displayQueueIndex);

        vkCmdPipelineBarrier(virtualFrame->cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
    }

    vkEndCommandBuffer(virtualFrame->cmdBuffer);

    return 0;
}

int Renderer::draw(DeviceInfo device) {
    this->currentVirtualFrame = (this->currentVirtualFrame + 1) % this->virtualFrameCount;
    uint32_t imageIndex = 0;
    vkWaitForFences(device.logical, 1, &this->virtualFrames[currentVirtualFrame].fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device.logical, 1, &this->virtualFrames[currentVirtualFrame].fence);

    VkResult result = vkAcquireNextImageKHR(device.logical, this->swapchain.getHandle(), UINT64_MAX,
                                            this->virtualFrames[currentVirtualFrame].imageAvailableSema, VK_NULL_HANDLE,
                                            &imageIndex);
    switch (result) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            break;
        default:
            return result;
    }

    prepareVirtualFrame(device, &this->virtualFrames[currentVirtualFrame], this->swapchain.getExtent(),
                        &this->swapchain.getImageViews()[imageIndex], this->swapchain.getImages()[imageIndex]);

    VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo submitInfo = VKSTRUCT::submitInfo(
            1, &this->virtualFrames[currentVirtualFrame].cmdBuffer,
            1, &this->virtualFrames[currentVirtualFrame].imageFinishProcessingSema,
            1, &this->virtualFrames[currentVirtualFrame].imageAvailableSema,
            &stageFlags);

    vkQueueSubmit(device.graphicQueue, 1, &submitInfo, this->virtualFrames[currentVirtualFrame].fence);

    VkSwapchainKHR swapchainKHR = this->swapchain.getHandle();

    VkPresentInfoKHR presentInfo = VKSTRUCT::presentInfoKhr(1, &swapchainKHR, &imageIndex, 1, &this->virtualFrames[currentVirtualFrame].imageFinishProcessingSema);

    result = vkQueuePresentKHR(device.displayQueue, &presentInfo);

    switch (result) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            break;
        default:
            return result;
    }
    return 0;
}

int Renderer::initVirtualFrames(DeviceInfo device) {
    VkCommandBuffer cmdBuffers[virtualFrameCount];
    VkCommandBufferAllocateInfo allocateInfo = VKSTRUCT::commandBufferAllocateInfo(virtualFrameCount, this->cmdPool);
    vkAllocateCommandBuffers(device.logical, &allocateInfo, cmdBuffers);

    VkSemaphoreCreateInfo semaphoreCreateInfo = VKSTRUCT::semaphoreCreateInfo();
    VkFenceCreateInfo fenceCreateInfo = VKSTRUCT::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

    for (int i = 0; i < virtualFrameCount; i++) {
        this->virtualFrames[i].cmdBuffer = cmdBuffers[i];
        this->virtualFrames[i].framebuffer = VK_NULL_HANDLE;
        vkCreateSemaphore(device.logical, &semaphoreCreateInfo, nullptr,
                          &this->virtualFrames[i].imageFinishProcessingSema);
        vkCreateSemaphore(device.logical, &semaphoreCreateInfo, nullptr, &this->virtualFrames[i].imageAvailableSema);
        vkCreateFence(device.logical, &fenceCreateInfo, nullptr, &this->virtualFrames[i].fence);
    }
    return 0;
}

int Renderer::initCommandPool(DeviceInfo device) {
    VkCommandPoolCreateInfo commandPoolCreateInfo = VKSTRUCT::commandPoolCreateInfo(device.graphicQueueIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    vkCreateCommandPool(device.logical, &commandPoolCreateInfo, nullptr, &this->cmdPool);
    return 0;
}

int Renderer::initRenderer(DeviceInfo device, VkSurfaceKHR surface) {
    this->swapchain.initSwapchain(device, surface);
    this->initRenderPass(device.logical, this->swapchain.getSurfaceFormat().format);
    this->initCommandPool(device);
    this->initVirtualFrames(device);
    this->initResources(device, "img/texture.png", &this->texture);
    this->initGraphicPipeline(device);
    return 0;
}

int Renderer::updateStagingBuffer(DeviceInfo device, const void *data, size_t size) {
    void *ptrBuffer = this->hostVisibleMemory.getMappedMemory();
    memcpy(ptrBuffer, data, size);
    VkMappedMemoryRange memoryRange = VKSTRUCT::mappedMemoryRange(this->hostVisibleMemory.getHandle());
    vkFlushMappedMemoryRanges(device.logical, 1, &memoryRange);
    return 0;
}

Renderer::Renderer() {
    this->currentVirtualFrame = 0;
}

int Renderer::createImage(DeviceInfo device, uint32_t width, uint32_t height, VkImage *image) {
    VkImageCreateInfo imageCreateInfo = VKSTRUCT::imageCreateInfo(width, height);
    return vkCreateImage(device.logical, &imageCreateInfo, nullptr, image) != VK_SUCCESS;
}

int Renderer::allocateMemory(DeviceInfo device, VkMemoryRequirements memoryRequirements,
                             VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceMemory *memory) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(device.physical, &memoryProperties);

    uint32_t memoryTypeIndex = -1;
    for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (memoryRequirements.memoryTypeBits & (1 << i)
            && (memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
            memoryTypeIndex = i;
            break;
        }
    }
    if (memoryTypeIndex == -1) {
        std::cout << "Didn't find memory for allocation" << std::endl;
    }

    VkMemoryAllocateInfo memoryAllocateInfo = VKSTRUCT::memoryAllocateInfo(memoryRequirements.size, memoryTypeIndex);
    vkAllocateMemory(device.logical, &memoryAllocateInfo, nullptr, memory);
    return 0;
}

int Renderer::allocateImageMemory(DeviceInfo device, VkImage image, VkMemoryPropertyFlags memoryPropertyFlags,
                                  VkDeviceMemory *memory) {
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device.logical, image, &memoryRequirements);
    return this->allocateMemory(device, memoryRequirements, memoryPropertyFlags, memory);
}

int Renderer::createImageView(DeviceInfo device, VkImage image, VkImageView *imageView) {
    VkImageViewCreateInfo createInfo = VKSTRUCT::imageViewCreateInfo(image);
    vkCreateImageView(device.logical, &createInfo, nullptr, imageView);
    return 0;
}

int Renderer::updateTexture(DeviceInfo device, ImageFile imageFile, VkImage image) {
    this->updateStagingBuffer(device, imageFile.data, imageFile.size);

    this->currentVirtualFrame = (this->currentVirtualFrame + 1) % this->virtualFrameCount;
    vkWaitForFences(device.logical, 1, &this->virtualFrames[currentVirtualFrame].fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device.logical, 1, &this->virtualFrames[currentVirtualFrame].fence);

    VirtualFrame virtualFrame = this->virtualFrames[this->currentVirtualFrame];

    VkCommandBufferBeginInfo beginInfo = VKSTRUCT::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    vkBeginCommandBuffer(this->virtualFrames[this->currentVirtualFrame].cmdBuffer, &beginInfo);

    VkImageSubresourceRange subresourceRange = VKSTRUCT::imageSubresourceRange(1, 0, 1, 0, VK_IMAGE_ASPECT_COLOR_BIT);

    VkImageMemoryBarrier imageMemoryBarrier = VKSTRUCT::imageMemoryBarrier(
            image, 0, VK_ACCESS_TRANSFER_WRITE_BIT, subresourceRange,VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    vkCmdPipelineBarrier(virtualFrame.cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                         0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    VkBufferImageCopy bufferImageCopy = VKSTRUCT::bufferImageCopy({imageFile.width, imageFile.height, 1});
    vkCmdCopyBufferToImage(virtualFrame.cmdBuffer, this->stagingBuffer.getHandle(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &bufferImageCopy);

    // Alter the previous image memory barrier to move to sample layout
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(virtualFrame.cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    vkEndCommandBuffer(virtualFrame.cmdBuffer);


    VkSubmitInfo submitInfo = VKSTRUCT::submitInfo(1, &virtualFrame.cmdBuffer, 0, nullptr, 0, nullptr, nullptr);
    vkQueueSubmit(device.graphicQueue, 1, &submitInfo, virtualFrame.fence);
    return 0;
}

int Renderer::initSampler(DeviceInfo device, VkSampler *sampler) {
    VkSamplerCreateInfo createInfo = VKSTRUCT::samplerCreateInfo();
    vkCreateSampler(device.logical, &createInfo, nullptr, sampler);
    return 0;
}

int Renderer::initDescriptorSetLayout(DeviceInfo device, VkDescriptorSetLayout *descriptorSetLayout) {
    VkDescriptorSetLayoutBinding bindings[3];
    bindings[0] = VKSTRUCT::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    bindings[1] = VKSTRUCT::descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    bindings[2] = VKSTRUCT::descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

    VkDescriptorSetLayoutCreateInfo createInfo = VKSTRUCT::descriptorSetLayoutCreateInfo(3, bindings);
    vkCreateDescriptorSetLayout(device.logical, &createInfo, nullptr, descriptorSetLayout);
    return 0;
}

int Renderer::initDescriptorPool(DeviceInfo device, VkDescriptorPool *descriptorPool) {
    VkDescriptorPoolSize descriptorPoolSize[] = {
            VKSTRUCT::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1),
            VKSTRUCT::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2)
    };

    VkDescriptorPoolCreateInfo createInfo = VKSTRUCT::descriptorPoolCreateInfo(1, 2, descriptorPoolSize);
    vkCreateDescriptorPool(device.logical, &createInfo, nullptr, descriptorPool);
    return 0;
}

int Renderer::allocateDescriptor(DeviceInfo device, VkDescriptorPool descriptorPool,
                                 VkDescriptorSetLayout *descriptorLayout, VkDescriptorSet *descriptorSet) {
    VkDescriptorSetAllocateInfo allocateInfo = VKSTRUCT::descriptorSetAllocateInfo(descriptorPool, 1, descriptorLayout);
    vkAllocateDescriptorSets(device.logical, &allocateInfo, descriptorSet);
    return 0;
}

int
Renderer::updateDescriptor(DeviceInfo device, VkDescriptorSet descriptorSet, VkImageView imageView, VkSampler sampler, Buffer uniformBuffer, Buffer model) {

    VkDescriptorImageInfo imageInfo = VKSTRUCT::descriptorImageInfo(sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, imageView);
    VkDescriptorBufferInfo bufferInfos[2];
    bufferInfos[0] = VKSTRUCT::descriptorBufferInfo(uniformBuffer.getHandle());
    bufferInfos[1] = VKSTRUCT::descriptorBufferInfo(model.getHandle());

    VkWriteDescriptorSet writeDescriptorSet[3];
    writeDescriptorSet[0] = VKSTRUCT::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, 1, &imageInfo);
    writeDescriptorSet[1] = VKSTRUCT::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, 1, &bufferInfos[0]);
    writeDescriptorSet[2] = VKSTRUCT::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, 1, &bufferInfos[1]);

    vkUpdateDescriptorSets(device.logical, 3, writeDescriptorSet, 0, nullptr);
    return 0;
}

int Renderer::initResources(DeviceInfo device, const char *filename, CombinedImageSampler *texture) {
    // RESOURCES FOR COMBINED IMAGE SAMPLER
    // Load texture file
    ImageFile imageFile;
    FileReader::loadImage(filename, &imageFile);

    // Initialise texture
    initTexture(device, imageFile.width, imageFile.height, &texture->image);

    // Create Sampler
    initSampler(device, &this->texture.sampler);

    // Buffers
    Buffer deviceLocalBuffers[5];
    deviceLocalBuffers[0] = this->vertexBuffer  = Buffer::createBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(Data::indexedVertexData));
    deviceLocalBuffers[1] = this->indexBuffer   = Buffer::createBuffer(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(Data::indexData));
    deviceLocalBuffers[2] = this->uniformBuffer = Buffer::createBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(float) * 16);
    Buffer modelMatrix = deviceLocalBuffers[3] = Buffer::createBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(float) * 16 * 2);
    deviceLocalBuffers[4] = this->instanceBuffer = Buffer::createBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(float) * 4);

    this->stagingBuffer = Buffer::createBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1000000);

    // Memory objects
    this->hostVisibleMemory = DeviceMemory::createHostVisibleMemory(device, 1, &this->stagingBuffer);
    this->deviceLocalMemory = DeviceMemory::createDeviceMemory(device, 5, deviceLocalBuffers,
                                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Initialise new descriptor set
    initDescriptorSet(device, &this->descriptorSets[0]);
    // Update descriptor
    updateDescriptor(device, this->descriptorSets[0].handle, this->texture.image.view, this->texture.sampler, this->uniformBuffer, modelMatrix);

    // Update texture
    updateTexture(device, imageFile, texture->image.handle);

    // update uniform buffer
    GMATH::mat4 orthoMat = GMATH::orthographicMatrix(-30, 30, -30, 30, 0.0, 1.0);
    updateStagingBuffer(device, &orthoMat, sizeof(orthoMat));
    submitStagingBuffer(device, VK_ACCESS_UNIFORM_READ_BIT, this->uniformBuffer, sizeof(orthoMat));
    vkDeviceWaitIdle(device.logical);

    GMATH::mat4 idMat[2];
    idMat[0] = GMATH::translateMatrix();
    idMat[1] = GMATH::identityMatrix();

    updateStagingBuffer(device, idMat, sizeof(float) * 16 * 2);
    submitStagingBuffer(device, VK_ACCESS_UNIFORM_READ_BIT, modelMatrix, sizeof(float) * 16 * 2);
    vkDeviceWaitIdle(device.logical);

    // TODO: Use more granular synchro for device wait idle after every update, problem is that we reuse the same staging buffer each update

    // Update vertex buffer
    updateStagingBuffer(device, Data::indexedVertexData, sizeof(Data::indexedVertexData));
    submitStagingBuffer(device, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, this->vertexBuffer, sizeof(Data::indexedVertexData));
    vkDeviceWaitIdle(device.logical);

    // Update instance buffer
    updateStagingBuffer(device, Data::instanceData, sizeof(Data::instanceData));
    submitStagingBuffer(device, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, this->instanceBuffer, sizeof(Data::instanceData));
    vkDeviceWaitIdle(device.logical);

    // Update index buffer
    updateStagingBuffer(device, Data::indexData, sizeof(Data::indexData));
    submitStagingBuffer(device, VK_ACCESS_INDEX_READ_BIT, this->indexBuffer, sizeof(Data::indexData));
    vkDeviceWaitIdle(device.logical);


    // Free loaded texture image file
    FileReader::freeImage(&imageFile);
    return 0;
}

int Renderer::initDescriptorSet(DeviceInfo device, DescriptorSet *descriptorSet) {
    initDescriptorSetLayout(device, &descriptorSet->layout);
    initDescriptorPool(device, &descriptorSet->pool);
    allocateDescriptor(device, descriptorSet->pool, &descriptorSet->layout, &descriptorSet->handle);
    return 0;
}

int Renderer::initTexture(DeviceInfo device, uint32_t width, uint32_t height, Image *texture) {
    createImage(device, width, height, &texture->handle);
    allocateImageMemory(device, texture->handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->memory);
    vkBindImageMemory(device.logical, texture->handle, texture->memory, 0);
    createImageView(device, texture->handle, &texture->view);
    return 0;
}

int Renderer::windowResize(DeviceInfo device, VkSurfaceKHR surface) {
    this->swapchain.recreateSwapchain(device, surface);
    return 0;
}

int Renderer::submitStagingBuffer(DeviceInfo device, VkAccessFlagBits dstBufferAccessFlags, Buffer dstBuffer, uint64_t sizeOfData) {
    this->currentVirtualFrame = (this->currentVirtualFrame + 1) % this->virtualFrameCount;
    vkWaitForFences(device.logical, 1, &this->virtualFrames[currentVirtualFrame].fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device.logical, 1, &this->virtualFrames[currentVirtualFrame].fence);

    VkCommandBufferBeginInfo beginInfo = VKSTRUCT::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    vkBeginCommandBuffer(this->virtualFrames[this->currentVirtualFrame].cmdBuffer, &beginInfo);

    VkBufferCopy bufferCopy = VKSTRUCT::bufferCopy(sizeOfData, 0, 0);
    vkCmdCopyBuffer(this->virtualFrames[this->currentVirtualFrame].cmdBuffer, this->stagingBuffer.getHandle(), dstBuffer.getHandle(),
                    1, &bufferCopy);

    // This is undefined behaviour... will need to fix! As in commenting it out is undefined
    /*
    VkBufferMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    memoryBarrier.pNext = nullptr;
    memoryBarrier.size = VK_WHOLE_SIZE;
    memoryBarrier.buffer = dstBuffer;
    memoryBarrier.offset = 0;
    memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    memoryBarrier.dstAccessMask = dstBufferAccessFlags;
    vkCmdPipelineBarrier(this->virtualFrames[this->currentVirtualFrame].cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1, &memoryBarrier, 0, nullptr);
    */

    vkEndCommandBuffer(this->virtualFrames[this->currentVirtualFrame].cmdBuffer);
    VkSubmitInfo submitInfo = VKSTRUCT::submitInfo(1, &this->virtualFrames[this->currentVirtualFrame].cmdBuffer, 0, nullptr, 0, nullptr, nullptr);
    vkQueueSubmit(device.graphicQueue, 1, &submitInfo, this->virtualFrames[this->currentVirtualFrame].fence);
    return 0;
}
