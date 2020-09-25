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
    VkAttachmentDescription attachments[2] = {
            VKSTRUCT::colorAttachmentDescription(format),
            VKSTRUCT::depthAttachmentDescription(this->depth.image.format)
    };

    VkAttachmentReference colorAttachmentRef = VKSTRUCT::attachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkAttachmentReference depthAttachmentRef = VKSTRUCT::attachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    VkSubpassDescription subpassDescriptions = VKSTRUCT::subpassDescription(0, nullptr, 1, &colorAttachmentRef, &depthAttachmentRef);

    VkSubpassDependency subpassDependencies[] = {
            VKSTRUCT::subpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_DEPENDENCY_BY_REGION_BIT),
            VKSTRUCT::subpassDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                        VK_ACCESS_MEMORY_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT)
    };

    VkRenderPassCreateInfo createInfo = VKSTRUCT::renderPassCreateInfo(2, attachments, 2, subpassDependencies, 1, &subpassDescriptions);
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

    // vkDestroyImageView(device.logical, this->texture.image.view, nullptr);
    // vkDestroyImage(device.logical, this->texture.image.handle, nullptr);
    // vkFreeMemory(device.logical, this->texture.image.memory, nullptr);

    // vkDestroySampler(device.logical, this->texture.sampler, nullptr);

    vkDestroyDescriptorSetLayout(device.logical, this->descriptorSets[0].layout, nullptr);
    vkDestroyDescriptorPool(device.logical, this->descriptorSets[0].pool, nullptr);

    if (this->pipelines.cubes.layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device.logical, this->pipelines.cubes.layout, nullptr);
    }
    if (this->pipelines.backdrop.layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device.logical, this->pipelines.backdrop.layout, nullptr);
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
    this->projectionBuffer.destroy(device);

    this->deviceLocalMemory.free(device);
    this->hostVisibleMemory.free(device);

    vkDestroyCommandPool(device.logical, this->cmdPool, nullptr);
    vkDestroyRenderPass(device.logical, this->renderPass, nullptr);
    vkDestroyPipeline(device.logical, this->pipelines.cubes.handle, nullptr);
    vkDestroyPipeline(device.logical, this->pipelines.backdrop.handle, nullptr);

    this->swapchain.clean(device);
}

int Renderer::createPipelines(DeviceInfo device) {

    /// Cube Pipeline ///

    VkShaderModule vertexShader, fragmentShader;
    initShaderModule(device.logical, "shaders/vert.spv", &vertexShader);
    initShaderModule(device.logical, "shaders/frag.spv", &fragmentShader);

    /* Vertex data arranged as =
     * {x, y, z, w, r, g, b, a}
     * i.e. 4 32bit floats for xyzw coordinates and then rgba color values, interleaved
     * */

    VkVertexInputBindingDescription vertexInputBindingDescription[2];
    vertexInputBindingDescription[0] = VKSTRUCT::vertexInputBindingDescription(0, Data::Cube::vertexStride, VK_VERTEX_INPUT_RATE_VERTEX);      // Vertex data
    vertexInputBindingDescription[1] = VKSTRUCT::vertexInputBindingDescription(1, Data::Cube::instanceStride, VK_VERTEX_INPUT_RATE_INSTANCE);    // Instance Data

    VkVertexInputAttributeDescription vertexInputAttributeDescription[6];
    // Vertex
    vertexInputAttributeDescription[0] = VKSTRUCT::vertexInputAttributeDescription(0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, Data::Cube::positionOffset);
    vertexInputAttributeDescription[1] = VKSTRUCT::vertexInputAttributeDescription(0, VK_FORMAT_R32G32B32A32_SFLOAT, 1, Data::Cube::colourOffset);

    // Instance
    // We pass each column of our 4by4 mat as a separate vec4
    vertexInputAttributeDescription[2] = VKSTRUCT::vertexInputAttributeDescription(1, VK_FORMAT_R32G32B32A32_SFLOAT, 2, Data::Cube::instanceOffsets[0]);
    vertexInputAttributeDescription[3] = VKSTRUCT::vertexInputAttributeDescription(1, VK_FORMAT_R32G32B32A32_SFLOAT, 3, Data::Cube::instanceOffsets[1]);
    vertexInputAttributeDescription[4] = VKSTRUCT::vertexInputAttributeDescription(1, VK_FORMAT_R32G32B32A32_SFLOAT, 4, Data::Cube::instanceOffsets[2]);
    vertexInputAttributeDescription[5] = VKSTRUCT::vertexInputAttributeDescription(1, VK_FORMAT_R32G32B32A32_SFLOAT, 5, Data::Cube::instanceOffsets[3]);
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = VKSTRUCT::pipelineVertexInputStateCreateInfo(2, vertexInputBindingDescription, 6, vertexInputAttributeDescription);

    PipelineBuilder::createPipelineLayout(device, Renderer::descriptorSetCount, &this->descriptorSets[0].layout, &this->pipelines.cubes.layout);
    pipelineBuilder.setDepthStencil(VKSTRUCT::pipelineDepthStencilStateCreateInfo())->buildPipeline(device, this->renderPass, this->pipelines.cubes.layout, vertexInputStateCreateInfo, vertexShader, fragmentShader, &this->pipelines.cubes.handle);

    vkDestroyShaderModule(device.logical, vertexShader, nullptr);
    vkDestroyShaderModule(device.logical, fragmentShader, nullptr);

    /// Backdrop pipeline ///

    initShaderModule(device.logical, "shaders/vertexBack.spv", &vertexShader);
    initShaderModule(device.logical, "shaders/fragmentBack.spv", &fragmentShader);
    vertexInputStateCreateInfo = VKSTRUCT::pipelineVertexInputStateCreateInfo(0, nullptr, 0, nullptr);
    PipelineBuilder::createPipelineLayout(device, 0, nullptr, &this->pipelines.backdrop.layout);
    pipelineBuilder.buildPipeline(device, this->renderPass, this->pipelines.backdrop.layout, vertexInputStateCreateInfo, vertexShader, fragmentShader, &this->pipelines.backdrop.handle);

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
    VkImageView attachments[2] = {*imageView, this->depth.image.view};
    VkFramebufferCreateInfo framebufferCreateInfo = VKSTRUCT::framebufferCreateInfo(this->renderPass, 2, attachments, extent.width, extent.height);

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

    VkClearValue clearColor[2] = {};
    clearColor[0].color = {1.0f, 0.8f, 0.4f, 0.0f};
    clearColor[1].depthStencil = {1.0f, 0};

    VkViewport viewport = VKSTRUCT::viewport(extent.width, extent.height, 0, 0, 0.0f, 1.0f);

    VkRect2D rect = VKSTRUCT::rect2D({0, 0}, extent);

    vkCmdSetViewport(virtualFrame->cmdBuffer, 0, 1, &viewport);
    vkCmdSetScissor(virtualFrame->cmdBuffer, 0, 1, &rect);

    VkRenderPassBeginInfo renderPassBeginInfo = VKSTRUCT::renderPassBeginInfo(virtualFrame->framebuffer, this->renderPass, extent, 2, clearColor);

    vkCmdBeginRenderPass(virtualFrame->cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(virtualFrame->cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines.backdrop.handle);
    vkCmdDraw(virtualFrame->cmdBuffer, 6, 1, 0, 0);

    vkCmdBindPipeline(virtualFrame->cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines.cubes.handle);

    VkDeviceSize offset[] = {0ull, 0ull};
    VkBuffer buffers[2];
    buffers[0] = this->vertexBuffer.getHandle();
    buffers[1] = this->instanceBuffer.getHandle();

    vkCmdBindVertexBuffers(virtualFrame->cmdBuffer, 0, 2, buffers, offset);

    vkCmdBindIndexBuffer(virtualFrame->cmdBuffer, this->indexBuffer.getHandle(), 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(virtualFrame->cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines.cubes.layout, 0,
                            this->descriptorSetCount, &this->descriptorSets[0].handle, 0, nullptr);

    vkCmdDrawIndexed(virtualFrame->cmdBuffer, Data::Cube::cubeIndexCount, Data::Cube::instanceCount, 0, 0, 0);

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
    VirtualFrame *currentVFrame = &this->virtualFrames[currentVirtualFrame];
    uint32_t imageIndex = 0;

    vkWaitForFences(device.logical, 1, &currentVFrame->fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device.logical, 1, &currentVFrame->fence);

    switch (this->swapchain.acquireImage(device, &imageIndex, currentVFrame->imageAvailableSema)) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            break;
        default:
            return -1;
    }

    prepareVirtualFrame(device, currentVFrame, this->swapchain.getExtent(),
                        &this->swapchain.getImageViews()[imageIndex], this->swapchain.getImages()[imageIndex]);

    VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo submitInfo = VKSTRUCT::submitInfo(
            1, &currentVFrame->cmdBuffer,
            1, &currentVFrame->imageFinishProcessingSema,
            1, &currentVFrame->imageAvailableSema,
            &stageFlags);

    vkQueueSubmit(device.graphicQueue, 1, &submitInfo, currentVFrame->fence);

    switch (this->swapchain.presentImage(device, imageIndex, currentVFrame->imageFinishProcessingSema)) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            break;
        default:
            return -1;
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
    std::cout << "WHaytatduw" << std::endl;
    this->initDepthTestingResources(device, this->swapchain.getExtent().width, this->swapchain.getExtent().height);
    std::cout << "YOOOOOOOO" << std::endl;
    this->initRenderPass(device.logical, this->swapchain.getSurfaceFormat().format);
    this->initCommandPool(device);
    this->initVirtualFrames(device);
    this->initResources(device, "img/texture.png", &this->texture);
    this->createPipelines(device);
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
    VkImageCreateInfo imageCreateInfo = VKSTRUCT::imageCreateInfo(width, height, VK_FORMAT_R8G8B8A8_UNORM,
                                                                  static_cast<VkImageUsageFlagBits>(
                                                                          VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                          VK_IMAGE_USAGE_SAMPLED_BIT));
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
    VkImageViewCreateInfo createInfo = VKSTRUCT::imageViewCreateInfo(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
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
    // bindings[0] = VKSTRUCT::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    bindings[0] = VKSTRUCT::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    // bindings[2] = VKSTRUCT::descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

    VkDescriptorSetLayoutCreateInfo createInfo = VKSTRUCT::descriptorSetLayoutCreateInfo(1, bindings);
    vkCreateDescriptorSetLayout(device.logical, &createInfo, nullptr, descriptorSetLayout);
    return 0;
}

int Renderer::initDescriptorPool(DeviceInfo device, VkDescriptorPool *descriptorPool) {
    VkDescriptorPoolSize descriptorPoolSize[] = {
            // VKSTRUCT::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1),
            VKSTRUCT::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1)
    };

    VkDescriptorPoolCreateInfo createInfo = VKSTRUCT::descriptorPoolCreateInfo(1, 1, descriptorPoolSize);
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
Renderer::updateDescriptor(DeviceInfo device, VkDescriptorSet descriptorSet, VkImageView imageView, VkSampler sampler, Buffer uniformBuffer, Buffer model = Buffer()) {

    VkDescriptorImageInfo imageInfo = VKSTRUCT::descriptorImageInfo(sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, imageView);
    VkDescriptorBufferInfo bufferInfos[2];
    bufferInfos[0] = VKSTRUCT::descriptorBufferInfo(uniformBuffer.getHandle());
    //bufferInfos[1] = VKSTRUCT::descriptorBufferInfo(model.getHandle());

    VkWriteDescriptorSet writeDescriptorSet[3];
    writeDescriptorSet[0] = VKSTRUCT::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, 1, &imageInfo);
    writeDescriptorSet[1] = VKSTRUCT::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, 1, &bufferInfos[0]);
    //writeDescriptorSet[2] = VKSTRUCT::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, 1, &bufferInfos[1]);

    vkUpdateDescriptorSets(device.logical, 2, writeDescriptorSet, 0, nullptr);
    return 0;
}

int Renderer::initResources(DeviceInfo device, const char *filename, CombinedImageSampler *texture) {
    // RESOURCES FOR COMBINED IMAGE SAMPLER
    // Load texture file
    // ImageFile imageFile;
    // FileReader::loadImage(filename, &imageFile);

    // Initialise texture
    // initTexture(device, imageFile.width, imageFile.height, &texture->image);

    // Create Sampler
    // initSampler(device, &this->texture.sampler);

    // Buffers
    Buffer deviceLocalBuffers[5];
    deviceLocalBuffers[0] = this->vertexBuffer  = Buffer::createBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(Data::Cube::cubeModel));
    deviceLocalBuffers[1] = this->indexBuffer   = Buffer::createBuffer(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(Data::Cube::cubeIndex));
    deviceLocalBuffers[2] = this->instanceBuffer = Buffer::createBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(Data::Cube::instanceViews));

    deviceLocalBuffers[3] = this->projectionBuffer = Buffer::createBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(float) * 16);
    // Buffer modelMatrix = deviceLocalBuffers[3] = Buffer::createBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(float) * 16 * 2);
    // deviceLocalBuffers[4] = this->instanceBuffer = Buffer::createBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(float) * Data::instanceCount * 2);

    this->stagingBuffer = Buffer::createBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1000000);

    // Memory objects
    this->hostVisibleMemory = DeviceMemory::createHostVisibleMemory(device, 1, &this->stagingBuffer);
    this->deviceLocalMemory = DeviceMemory::createDeviceMemory(device, 4, deviceLocalBuffers,
                                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Initialise new descriptor set
    initDescriptorSet(device, &this->descriptorSets[0]);
    // Update descriptor
    // updateDescriptor(device, this->descriptorSets[0].handle, this->texture.image.view, this->texture.sampler, this->projectionBuffer);

    VkDescriptorBufferInfo bufferInfos[1];
    bufferInfos[0] = VKSTRUCT::descriptorBufferInfo(this->projectionBuffer.getHandle());
    VkWriteDescriptorSet writeDescriptorSet[1];
    writeDescriptorSet[0] = VKSTRUCT::writeDescriptorSet(this->descriptorSets[0].handle, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, 1, &bufferInfos[0]);
    vkUpdateDescriptorSets(device.logical, 1, writeDescriptorSet, 0, nullptr);


    // Update texture
    // updateTexture(device, imageFile, texture->image.handle);

    // update uniform buffer
    GMATH::mat4 orthoMat = GMATH::orthographicMatrix(-30, 30, -30, 30, -10, 10);
    updateStagingBuffer(device, &orthoMat, sizeof(orthoMat));
    submitStagingBuffer(device, VK_ACCESS_UNIFORM_READ_BIT, this->projectionBuffer, sizeof(orthoMat));
    vkDeviceWaitIdle(device.logical);

    /*
    GMATH::mat4 idMat[2];
    idMat[0] = GMATH::translateMatrix(GMATH::vec3(15.0f, -5.0f, 0.0f));
    idMat[1] = GMATH::identityMatrix();
    */

    // Update unform model matrices
    /*
    updateStagingBuffer(device, idMat, sizeof(float) * 16 * 2);
    submitStagingBuffer(device, VK_ACCESS_UNIFORM_READ_BIT, modelMatrix, sizeof(float) * 16 * 2);
    vkDeviceWaitIdle(device.logical);
    */

    // TODO: Use more granular synchro for device wait idle after every update, problem is that we reuse the same staging buffer each update

    // Update vertex buffer
    updateStagingBuffer(device, Data::Cube::cubeModel, sizeof(Data::Cube::cubeModel));
    submitStagingBuffer(device, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, this->vertexBuffer, sizeof(Data::Cube::cubeModel));
    vkDeviceWaitIdle(device.logical);

    // Update instance buffer
    updateStagingBuffer(device, Data::Cube::instanceViews, sizeof(Data::Cube::instanceViews));
    submitStagingBuffer(device, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, this->instanceBuffer, sizeof(Data::Cube::instanceViews));
    vkDeviceWaitIdle(device.logical);

    // Update index buffer
    updateStagingBuffer(device, Data::Cube::cubeIndex, sizeof(Data::Cube::cubeIndex));
    submitStagingBuffer(device, VK_ACCESS_INDEX_READ_BIT, this->indexBuffer, sizeof(Data::Cube::cubeIndex));
    vkDeviceWaitIdle(device.logical);


    // Free loaded texture image file
    // FileReader::freeImage(&imageFile);
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
    allocateImageMemory(device, texture->handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->textureMemory);
    vkBindImageMemory(device.logical, texture->handle, this->textureMemory, 0);
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

VkFormat Renderer::getSuitableFormat(DeviceInfo device, uint32_t candidateCount, VkFormat *candidateFormats, VkFormatFeatureFlagBits requiredFormatFeatures) {
    for (uint32_t i = 0; i < candidateCount; i++) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(device.physical, candidateFormats[i], &properties);

        if ((properties.optimalTilingFeatures & requiredFormatFeatures) == requiredFormatFeatures) {
            return candidateFormats[i];
        }
    }

    throw std::runtime_error("No suitable formats available");
}

int Renderer::initDepthTestingResources(DeviceInfo device, uint32_t width, uint32_t height) {
    VkFormat candidateFormats[3] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    this->depth.image.format = getSuitableFormat(device,3, candidateFormats, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    VkImageCreateInfo imageCreateInfo = VKSTRUCT::imageCreateInfo(width, height, this->depth.image.format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    vkCreateImage(device.logical, &imageCreateInfo, nullptr, &this->depth.image.handle);
    this->depth.memory = DeviceMemory::createDeviceMemory(device, 1, &this->depth.image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkImageViewCreateInfo imageViewCreateInfo = VKSTRUCT::imageViewCreateInfo(this->depth.image.handle, this->depth.image.format, VK_IMAGE_ASPECT_DEPTH_BIT);
    vkCreateImageView(device.logical, &imageViewCreateInfo, nullptr, &this->depth.image.view);

    return 0;
}
