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
    VkAttachmentDescription attachmentDescriptions[1];
    attachmentDescriptions[0].format = format;
    attachmentDescriptions[0].flags = 0;
    attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;

    VkAttachmentReference colorAttachmentReferences[1];
    colorAttachmentReferences[0].attachment = 0;
    colorAttachmentReferences[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescriptions[1];
    subpassDescriptions[0].flags = 0;
    subpassDescriptions[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescriptions[0].inputAttachmentCount = 0;
    subpassDescriptions[0].pInputAttachments = nullptr;
    subpassDescriptions[0].colorAttachmentCount = 1;
    subpassDescriptions[0].pColorAttachments = colorAttachmentReferences;
    subpassDescriptions[0].pResolveAttachments = nullptr;
    subpassDescriptions[0].pDepthStencilAttachment = nullptr;
    subpassDescriptions[0].preserveAttachmentCount = 0;
    subpassDescriptions[0].pPreserveAttachments = nullptr;

    VkSubpassDependency subpassDependencies[2];
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.pNext = nullptr;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = attachmentDescriptions;
    createInfo.dependencyCount = 2;
    createInfo.pDependencies = subpassDependencies;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = subpassDescriptions;

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

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = prog.size;
    createInfo.pCode = reinterpret_cast<uint32_t *>(prog.data);

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
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2];
    shaderStageCreateInfo[0] = VKSTRUCT::pipelineShaderStageCreateInfo(vertexShader, VK_SHADER_STAGE_VERTEX_BIT);
    shaderStageCreateInfo[1] = VKSTRUCT::pipelineShaderStageCreateInfo(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT);

    /* Vertex data arranged as =
     * {x, y, z, w, r, g, b, a}
     * i.e. 4 32bit floats for xyzw coordinates and then rgba color values, interleaved
     * */

    VkVertexInputBindingDescription vertexInputBindingDescription[1];
    vertexInputBindingDescription[0] = VKSTRUCT::vertexInputBindingDescription(0, sizeof(float) * 6, VK_VERTEX_INPUT_RATE_VERTEX);
    VkVertexInputAttributeDescription vertexInputAttributeDescription[2];
    vertexInputAttributeDescription[0] = VKSTRUCT::vertexInputAttributeDescription(0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, Data::positionOffset);
    vertexInputAttributeDescription[1] = VKSTRUCT::vertexInputAttributeDescription(0, VK_FORMAT_R32G32_SFLOAT, 1, Data::colorOffset);
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = VKSTRUCT::pipelineVertexInputStateCreateInfo(1, vertexInputBindingDescription, 2, vertexInputAttributeDescription);
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

/*
int Renderer::initBuffersAndMemory(DeviceInfo device) {
  initBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1000000, &this->stagingBuffer);
  initBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, Data::vertexDataSize, &this->vertexBuffer);

    allocateBufferMemory(device, this->stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &this->hostVisibleMemory);
    allocateBufferMemory(device, this->vertexBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->deviceLocalMemory);

  vkBindBufferMemory(device.logical, this->stagingBuffer, this->hostVisibleMemory, 0);
  vkBindBufferMemory(device.logical, this->vertexBuffer, this->deviceLocalMemory, 0);

  updateStagingBuffer(device, Data::vertexData, Data::vertexDataSize);
  return submitStagingBuffer(device);
}
 */



int
Renderer::prepareVirtualFrame(DeviceInfo device, VirtualFrame *virtualFrame, VkExtent2D extent, VkImageView *imageView,
                              VkImage image) {
    if (virtualFrame->framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(device.logical, virtualFrame->framebuffer, nullptr);
    }

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.pNext = nullptr;
    framebufferCreateInfo.flags = 0;
    framebufferCreateInfo.renderPass = this->renderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = imageView;
    framebufferCreateInfo.width = extent.width;
    framebufferCreateInfo.height = extent.height;
    framebufferCreateInfo.layers = 1;

    vkCreateFramebuffer(device.logical, &framebufferCreateInfo, nullptr, &virtualFrame->framebuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    vkBeginCommandBuffer(virtualFrame->cmdBuffer, &beginInfo);

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.levelCount = 1;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.layerCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    if (device.graphicQueueIndex != device.displayQueueIndex) {
        VkImageMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        memoryBarrier.pNext = nullptr;
        memoryBarrier.image = image;
        memoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        memoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        memoryBarrier.srcQueueFamilyIndex = device.displayQueueIndex;
        memoryBarrier.dstQueueFamilyIndex = device.graphicQueueIndex;
        memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        memoryBarrier.newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        memoryBarrier.subresourceRange = subresourceRange;

        vkCmdPipelineBarrier(virtualFrame->cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
    }

    VkClearValue clearColor = {1.0f, 0.8f, 0.4f, 0.0f};
    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.framebuffer = virtualFrame->framebuffer;
    renderPassBeginInfo.renderPass = this->renderPass;
    renderPassBeginInfo.renderArea.extent = extent;
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(virtualFrame->cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(virtualFrame->cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);

    VkViewport viewport = {};
    viewport.height = extent.height;
    viewport.width = extent.width;
    viewport.x = 0;
    viewport.y = 0;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D rect = {};
    rect.offset.x = 0;
    rect.offset.y = 0;
    rect.extent = extent;

    vkCmdSetViewport(virtualFrame->cmdBuffer, 0, 1, &viewport);
    vkCmdSetScissor(virtualFrame->cmdBuffer, 0, 1, &rect);

    VkDeviceSize offset = 0;
    VkBuffer buffer = this->vertexBuffer.getHandle();
    vkCmdBindVertexBuffers(virtualFrame->cmdBuffer, 0, 1, &buffer, &offset);

    vkCmdBindIndexBuffer(virtualFrame->cmdBuffer, this->indexBuffer.getHandle(), 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(virtualFrame->cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayout, 0,
                            this->descriptorSetCount, &this->descriptorSets[0].handle, 0, nullptr);

    // Data::
    // vkCmdDraw(virtualFrame->cmdBuffer, 6, 1, 0, 0);
    vkCmdDrawIndexed(virtualFrame->cmdBuffer, 6, 2, 0, 0, 0);

    vkCmdEndRenderPass(virtualFrame->cmdBuffer);

    if (device.graphicQueueIndex != device.displayQueueIndex) {
        VkImageMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        memoryBarrier.pNext = nullptr;
        memoryBarrier.image = image;
        memoryBarrier.srcAccessMask = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        memoryBarrier.dstAccessMask = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        memoryBarrier.srcQueueFamilyIndex = device.graphicQueueIndex;
        memoryBarrier.dstQueueFamilyIndex = device.displayQueueIndex;
        memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        memoryBarrier.newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        memoryBarrier.subresourceRange = subresourceRange;

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
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->virtualFrames[currentVirtualFrame].cmdBuffer; // &this->cmdBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &this->virtualFrames[currentVirtualFrame].imageFinishProcessingSema;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &this->virtualFrames[currentVirtualFrame].imageAvailableSema;
    submitInfo.pWaitDstStageMask = &stageFlags;

    vkQueueSubmit(device.graphicQueue, 1, &submitInfo, this->virtualFrames[currentVirtualFrame].fence);

    VkSwapchainKHR swapchainKHR = this->swapchain.getHandle();

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchainKHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &this->virtualFrames[currentVirtualFrame].imageFinishProcessingSema;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

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
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandBufferCount = virtualFrameCount;
    allocateInfo.commandPool = this->cmdPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkAllocateCommandBuffers(device.logical, &allocateInfo, cmdBuffers);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

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
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = nullptr;
    commandPoolCreateInfo.flags =
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    commandPoolCreateInfo.queueFamilyIndex = device.graphicQueueIndex;
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

    // vkMapMemory(device.logical, this->hostVisibleMemory.getHandle(), 0, size, 0, &ptrBuffer);
    memcpy(ptrBuffer, data, size);

    VkMappedMemoryRange memoryRange = {};
    memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    memoryRange.pNext = nullptr;
    memoryRange.size = VK_WHOLE_SIZE;
    memoryRange.offset = 0;
    memoryRange.memory = this->hostVisibleMemory.getHandle();
    vkFlushMappedMemoryRanges(device.logical, 1, &memoryRange);
    // vkUnmapMemory(device.logical, this->hostVisibleMemory.getHandle());
    return 0;
}

int Renderer::initBuffer(DeviceInfo device, VkBufferUsageFlags bufferUsageFlags, size_t size, VkBuffer *buffer) {
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.size = size;
    createInfo.usage = bufferUsageFlags;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;       // Only used when we have concurrent sharing mode
    createInfo.pQueueFamilyIndices = nullptr;

    vkCreateBuffer(device.logical, &createInfo, nullptr, buffer);
    return 0;
}

Renderer::Renderer() {
    this->currentVirtualFrame = 0;
}

int Renderer::createImage(DeviceInfo device, uint32_t width, uint32_t height, VkImage *image) {
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM; //Todo: Fix format hardcode
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;

    return vkCreateImage(device.logical, &imageCreateInfo, nullptr, image) != VK_SUCCESS;
}

int Renderer::allocateBufferMemory(DeviceInfo device, int bufferCount, VkBuffer *buffers,
                                   VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceMemory *memory) {
    VkMemoryRequirements memoryRequirements;
    memoryRequirements.size = 0;
    memoryRequirements.memoryTypeBits = -1;
    memoryRequirements.alignment = 0;
    for (int i = 0; i < bufferCount; i++) {
        VkMemoryRequirements currentRequirement;
        vkGetBufferMemoryRequirements(device.logical, buffers[i], &currentRequirement);
        memoryRequirements.size +=
                (currentRequirement.alignment - memoryRequirements.size % currentRequirement.alignment) %
                currentRequirement.alignment + currentRequirement.size;
        memoryRequirements.memoryTypeBits &= currentRequirement.memoryTypeBits;
    }
    return this->allocateMemory(device, memoryRequirements, memoryPropertyFlags, memory);
}

int Renderer::allocateMemory(DeviceInfo device, VkMemoryRequirements memoryRequirements,
                             VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceMemory *memory) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(device.physical, &memoryProperties);

    uint32_t memeoryIndex = -1;
    for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (memoryRequirements.memoryTypeBits & (1 << i)
            && (memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
            memeoryIndex = i;
            break;
        }
    }
    if (memeoryIndex == -1) {
        std::cout << "Didn't find memory for allocation" << std::endl;
    }

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = nullptr;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memeoryIndex;

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
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.image = image;
    createInfo.format = VK_FORMAT_R8G8B8A8_UNORM; //Todo: Fix format hardcode
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.layerCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseMipLevel = 0;

    vkCreateImageView(device.logical, &createInfo, nullptr, imageView);
    return 0;
}

int Renderer::updateTexture(DeviceInfo device, ImageFile imageFile, VkImage image) {
    this->updateStagingBuffer(device, imageFile.data, imageFile.size);

    this->currentVirtualFrame = (this->currentVirtualFrame + 1) % this->virtualFrameCount;
    vkWaitForFences(device.logical, 1, &this->virtualFrames[currentVirtualFrame].fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device.logical, 1, &this->virtualFrames[currentVirtualFrame].fence);

    VirtualFrame virtualFrame = this->virtualFrames[this->currentVirtualFrame];

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    vkBeginCommandBuffer(this->virtualFrames[this->currentVirtualFrame].cmdBuffer, &beginInfo);

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.levelCount = 1;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.layerCount = 1;
    subresourceRange.baseArrayLayer = 0;

    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext = nullptr;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier.image = image; // TODO: Put the image for the texture
    imageMemoryBarrier.srcQueueFamilyIndex = device.graphicQueueIndex;
    imageMemoryBarrier.dstQueueFamilyIndex = device.graphicQueueIndex;
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(virtualFrame.cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                         0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    VkBufferImageCopy bufferImageCopy = {};
    bufferImageCopy.bufferOffset = 0;
    bufferImageCopy.bufferImageHeight = 0;
    bufferImageCopy.bufferRowLength = 0;
    bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferImageCopy.imageSubresource.layerCount = 1;
    bufferImageCopy.imageSubresource.baseArrayLayer = 0;
    bufferImageCopy.imageSubresource.mipLevel = 0;
    bufferImageCopy.imageOffset = {0, 0, 0};
    bufferImageCopy.imageExtent = {imageFile.width, imageFile.height, 1};

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


    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &virtualFrame.cmdBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;

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

    VkDescriptorPoolSize descriptorPoolSize[2];
    descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSize[0].descriptorCount = 1;

    descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSize[1].descriptorCount = 2;

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.maxSets = 1;
    createInfo.poolSizeCount = 2;
    createInfo.pPoolSizes = descriptorPoolSize;

    vkCreateDescriptorPool(device.logical, &createInfo, nullptr, descriptorPool);
    return 0;
}

int Renderer::allocateDescriptor(DeviceInfo device, VkDescriptorPool descriptorPool,
                                 VkDescriptorSetLayout *descriptorLayout, VkDescriptorSet *descriptorSet) {
    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = descriptorLayout;

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
    Buffer deviceLocalBuffers[4];
    deviceLocalBuffers[0] = this->vertexBuffer  = Buffer::createBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(Data::indexedVertexData));
    deviceLocalBuffers[1] = this->indexBuffer   = Buffer::createBuffer(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(Data::indexData));
    deviceLocalBuffers[2] = this->uniformBuffer = Buffer::createBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(float) * 16);
    Buffer modelMatrix = deviceLocalBuffers[3] = Buffer::createBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(float) * 16 * 2);

    this->stagingBuffer = Buffer::createBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1000000);

    // Memory objects
    this->hostVisibleMemory = DeviceMemory::createHostVisibleMemory(device, 1, &this->stagingBuffer);
    this->deviceLocalMemory = DeviceMemory::createDeviceMemory(device, 4, deviceLocalBuffers,
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

    // Update vertex buffer
    updateStagingBuffer(device, Data::indexedVertexData, sizeof(Data::indexedVertexData));
    submitStagingBuffer(device, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, this->vertexBuffer, sizeof(Data::indexedVertexData));
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

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    vkBeginCommandBuffer(this->virtualFrames[this->currentVirtualFrame].cmdBuffer, &beginInfo);

    VkBufferCopy bufferCopy = {};
    bufferCopy.size = sizeOfData;
    bufferCopy.srcOffset = 0;
    bufferCopy.dstOffset = 0;
    vkCmdCopyBuffer(this->virtualFrames[this->currentVirtualFrame].cmdBuffer, this->stagingBuffer.getHandle(), dstBuffer.getHandle(),
                    1, &bufferCopy);

    // This is undefined behaviour... will need to fix!
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

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->virtualFrames[this->currentVirtualFrame].cmdBuffer;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;

    vkQueueSubmit(device.graphicQueue, 1, &submitInfo, this->virtualFrames[this->currentVirtualFrame].fence);
    return 0;
}
