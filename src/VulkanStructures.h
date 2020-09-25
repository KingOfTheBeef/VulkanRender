//
// Created by jonat on 26/08/2020.
//

#ifndef DYNAMICLINK_VULKANSTRUCTURES_H
#define DYNAMICLINK_VULKANSTRUCTURES_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>

namespace VKSTRUCT {
    inline static VkPipelineShaderStageCreateInfo
    pipelineShaderStageCreateInfo(VkShaderModule shaderModule, VkShaderStageFlagBits shaderStage,
                                  const char *entryPoint = "main") {
        VkPipelineShaderStageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.module = shaderModule;
        info.stage = shaderStage;
        info.pName = entryPoint;
        info.pSpecializationInfo = nullptr;
        return info;
    }

    inline static VkVertexInputBindingDescription
    vertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate) {
        VkVertexInputBindingDescription description = {};
        description.binding = binding;
        description.stride = stride;
        description.inputRate = inputRate;
        return description;
    }

    inline static VkVertexInputAttributeDescription
    vertexInputAttributeDescription(uint32_t binding, VkFormat format, uint32_t location, uint32_t offset) {
        VkVertexInputAttributeDescription description = {};
        description.binding = binding;
        description.format = format; // Hard coded RGBA format for vertex buffers
        description.location = location;
        description.offset = offset;
        return description;
    }

    inline static VkPipelineVertexInputStateCreateInfo
    pipelineVertexInputStateCreateInfo(uint32_t vertexBindingDescriptionCount,
                                       VkVertexInputBindingDescription *vertexBindingDescriptions,
                                       uint32_t vertexAttributeDescriptionCount,
                                       VkVertexInputAttributeDescription *vertexAttributeDescriptions) {
        VkPipelineVertexInputStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.vertexBindingDescriptionCount = vertexBindingDescriptionCount;
        info.pVertexBindingDescriptions = vertexBindingDescriptions;
        info.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;
        info.pVertexAttributeDescriptions = vertexAttributeDescriptions;
        return info;
    }

    inline static VkPipelineInputAssemblyStateCreateInfo
    pipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable = VK_FALSE) {
        VkPipelineInputAssemblyStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.topology = topology;
        info.primitiveRestartEnable = primitiveRestartEnable;
        return info;
    }

    inline static VkPipelineViewportStateCreateInfo
    pipelineViewportStateCreateInfo(uint32_t viewportCount, uint32_t scissorCount) {
        VkPipelineViewportStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.viewportCount = viewportCount;
        info.pViewports = nullptr;
        info.scissorCount = scissorCount;
        info.pScissors = nullptr;
        return info;
    }

    inline static VkPipelineViewportStateCreateInfo
    pipelineViewportStateCreateInfo(uint32_t viewportCount, VkViewport *viewports, uint32_t scissorCount,
                                    VkRect2D *scissors) {
        VkPipelineViewportStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.viewportCount = viewportCount;
        info.pViewports = viewports;
        info.scissorCount = scissorCount;
        info.pScissors = scissors;
        return info;
    }

    inline static VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo() {
        VkPipelineRasterizationStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.rasterizerDiscardEnable = VK_FALSE;
        info.polygonMode = VK_POLYGON_MODE_FILL;
        info.cullMode = VK_CULL_MODE_NONE;
        info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        info.depthBiasEnable = VK_FALSE;
        info.depthBiasConstantFactor = 0.0f;
        info.depthBiasSlopeFactor = 0.0f;
        info.depthClampEnable = VK_FALSE;
        info.depthBiasClamp = 0.0f;
        info.lineWidth = 1.0f;
        return info;
    }

    inline static VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo() {
        VkPipelineMultisampleStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        info.sampleShadingEnable = VK_FALSE;
        info.minSampleShading = 1.0f;
        info.pSampleMask = nullptr;
        info.alphaToCoverageEnable = VK_FALSE;
        info.alphaToOneEnable = VK_FALSE;
        return info;
    }

    inline static VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState() {
        VkPipelineColorBlendAttachmentState state = {};
        state.blendEnable = VK_FALSE;
        state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        state.colorBlendOp = VK_BLEND_OP_ADD;
        state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        state.alphaBlendOp = VK_BLEND_OP_ADD;
        state.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        return state;
    }

    inline static VkPipelineColorBlendStateCreateInfo
    pipelineColorBlendStateCreateInfo(uint32_t colorBlendAttachmentCount,
                                      VkPipelineColorBlendAttachmentState *colorBlendAttachmentStates) {
        VkPipelineColorBlendStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.attachmentCount = colorBlendAttachmentCount;
        info.pAttachments = colorBlendAttachmentStates;
        info.logicOpEnable = VK_FALSE;
        info.logicOp = VK_LOGIC_OP_COPY;
        info.blendConstants[0] = 0.0f;
        info.blendConstants[1] = 0.0f;
        info.blendConstants[2] = 0.0f;
        info.blendConstants[3] = 0.0f;
        return info;
    }

    inline static VkPipelineLayoutCreateInfo
    pipelineLayoutCreateInfo(uint32_t descriptorSetLayoutCount, const VkDescriptorSetLayout *descriptorSetLayouts) {
        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.setLayoutCount = descriptorSetLayoutCount;
        info.pSetLayouts = descriptorSetLayouts;
        info.pushConstantRangeCount = 0;
        info.pPushConstantRanges = nullptr;
        return info;
    }

    inline static VkPipelineDynamicStateCreateInfo
    pipelineDynamicStateCreateInfo(uint32_t dynamicStateCount, VkDynamicState *dynamicStates) {
        VkPipelineDynamicStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.dynamicStateCount = dynamicStateCount;
        info.pDynamicStates = dynamicStates;
        return info;
    }

    inline static VkPipelineDepthStencilStateCreateInfo
    pipelineDepthStencilStateCreateInfo() {
        VkPipelineDepthStencilStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.depthTestEnable = VK_TRUE;
        info.depthWriteEnable = VK_TRUE;
        info.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        info.depthBoundsTestEnable = VK_FALSE;
        info.stencilTestEnable = VK_FALSE;
        info.front = {};
        info.back = {};
        info.minDepthBounds = 0.0f;
        info.maxDepthBounds = 1.0f;
        return info;
    }

    inline static VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
            const VkRenderPass renderPass,
            const VkPipelineVertexInputStateCreateInfo *vertexInputStateCreateInfo,
            const VkPipelineInputAssemblyStateCreateInfo *assemblyStateCreateInfo,
            const VkPipelineViewportStateCreateInfo *viewportStateCreateInfo,
            const VkPipelineRasterizationStateCreateInfo *rasterizationStateCreateInfo,
            const VkPipelineMultisampleStateCreateInfo *multisampleStateCreateInfo,
            const VkPipelineColorBlendStateCreateInfo *colorBlendStateCreateInfo,
            const VkPipelineLayout pipelineLayout,
            const VkPipelineDynamicStateCreateInfo *dynamicStateCreateInfo,
            const uint32_t shaderStageCount,
            const VkPipelineShaderStageCreateInfo *shaderStageCreateInfo,
            const VkPipelineDepthStencilStateCreateInfo *depthStencilStateCreateInfo) {
        VkGraphicsPipelineCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.renderPass = renderPass;
        info.layout = {};
        info.basePipelineHandle = VK_NULL_HANDLE;
        info.basePipelineIndex = -1;
        info.pVertexInputState = vertexInputStateCreateInfo;
        info.pInputAssemblyState = assemblyStateCreateInfo;
        info.pViewportState = viewportStateCreateInfo;
        info.pRasterizationState = rasterizationStateCreateInfo;
        info.pMultisampleState = multisampleStateCreateInfo;
        info.pColorBlendState = colorBlendStateCreateInfo;
        info.layout = pipelineLayout;
        info.pDepthStencilState = depthStencilStateCreateInfo;
        info.pDynamicState = dynamicStateCreateInfo;
        info.pTessellationState = nullptr;
        info.stageCount = 2;
        info.pStages = shaderStageCreateInfo;
        info.subpass = 0;
        return info;
    }

    inline static VkWriteDescriptorSet
    writeDescriptorSet(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType, uint32_t binding,
                       uint32_t descriptorCount, VkDescriptorImageInfo *imageInfos, uint32_t startingArrayElement = 0) {
        VkWriteDescriptorSet set;
        set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        set.pNext = nullptr;
        set.descriptorType = descriptorType;
        set.dstSet = descriptorSet;
        set.dstBinding = binding;
        set.descriptorCount = descriptorCount;
        set.dstArrayElement = startingArrayElement;
        set.pImageInfo = imageInfos;
        set.pBufferInfo = nullptr;
        set.pTexelBufferView = nullptr;
        return set;
    }

    inline static VkWriteDescriptorSet
    writeDescriptorSet(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType, uint32_t binding,
                       uint32_t descriptorCount, VkDescriptorBufferInfo *bufferInfos,
                       uint32_t startingArrayElement = 0) {
        VkWriteDescriptorSet set;
        set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        set.pNext = nullptr;
        set.descriptorType = descriptorType;
        set.dstSet = descriptorSet;
        set.dstBinding = binding;
        set.descriptorCount = descriptorCount;
        set.dstArrayElement = startingArrayElement;
        set.pBufferInfo = bufferInfos;
        set.pImageInfo = nullptr;
        set.pTexelBufferView = nullptr;
        return set;
    }

    inline static VkWriteDescriptorSet
    writeDescriptorSet(VkDescriptorSet descriptorSet, VkDescriptorType descriptorType, uint32_t binding,
                       uint32_t descriptorCount, VkBufferView *bufferViews, uint32_t startingArrayElement = 0) {
        VkWriteDescriptorSet set;
        set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        set.pNext = nullptr;
        set.descriptorType = descriptorType;
        set.dstSet = descriptorSet;
        set.dstBinding = binding;
        set.descriptorCount = descriptorCount;
        set.dstArrayElement = startingArrayElement;
        set.pTexelBufferView = bufferViews;
        set.pImageInfo = nullptr;
        set.pBufferInfo = nullptr;
        return set;
    }

    inline static VkDescriptorImageInfo
    descriptorImageInfo(VkSampler sampler, VkImageLayout imageLayout, VkImageView imageView) {
        VkDescriptorImageInfo info;
        info.sampler = sampler;
        info.imageLayout = imageLayout;
        info.imageView = imageView;
        return info;
    }

    inline static VkDescriptorBufferInfo
    descriptorBufferInfo(VkBuffer buffer, uint32_t offset = 0, VkDeviceSize deviceSize = VK_WHOLE_SIZE) {
        VkDescriptorBufferInfo info;
        info.buffer = buffer;
        info.offset = offset;
        info.range = deviceSize;
        return info;
    }

    inline struct VkSamplerCreateInfo samplerCreateInfo() {
        VkSamplerCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.magFilter = VK_FILTER_LINEAR;
        info.minFilter = VK_FILTER_LINEAR;
        info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        info.mipLodBias = 0.0f;
        info.anisotropyEnable = VK_FALSE;
        info.maxAnisotropy = 1.0f;
        info.compareEnable = VK_FALSE;
        info.compareOp = VK_COMPARE_OP_ALWAYS;
        info.minLod = 0.0f;
        info.maxLod = 0.0f;
        info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        info.unnormalizedCoordinates = VK_FALSE;
        return info;
    }

    inline static VkDescriptorSetLayoutBinding
    descriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
                               VkShaderStageFlagBits stageFlags) {
        VkDescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = descriptorCount;
        layoutBinding.stageFlags = stageFlags;
        layoutBinding.pImmutableSamplers = nullptr;
        return layoutBinding;
    }

    inline static VkDescriptorSetLayoutCreateInfo
    descriptorSetLayoutCreateInfo(uint32_t bindingCount, VkDescriptorSetLayoutBinding *descriptorSetLayoutBindings) {
        VkDescriptorSetLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.bindingCount = bindingCount;
        info.pBindings = descriptorSetLayoutBindings;
        return info;
    }

    inline static VkAttachmentDescription
    colorAttachmentDescription(VkFormat format, VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED) {
        VkAttachmentDescription description;
        description.format = format;
        description.flags = 0;
        description.initialLayout = initialLayout;
        description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        description.samples = VK_SAMPLE_COUNT_1_BIT;
        return description;
    }

    inline static VkAttachmentDescription
    depthAttachmentDescription(VkFormat format) {
        VkAttachmentDescription description;
        description.format = format;
        description.flags = 0;
        description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        description.samples = VK_SAMPLE_COUNT_1_BIT;
        return description;
    }

    inline static VkAttachmentReference attachmentReference(uint32_t attachment, VkImageLayout layout) {
        VkAttachmentReference reference;
        reference.attachment = attachment;
        reference.layout = layout;
        return reference;
    }

    inline static VkSubpassDescription subpassDescription(
            uint32_t inputAttachmentCount, VkAttachmentReference *inputAttachments,
            uint32_t colorAttachmentCount, VkAttachmentReference *colorAttachments,
            VkAttachmentReference *depthStencilAttachment) {
        VkSubpassDescription description;
        description.flags = 0;
        description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        description.inputAttachmentCount = inputAttachmentCount;
        description.pInputAttachments = inputAttachments;
        description.colorAttachmentCount = colorAttachmentCount;
        description.pColorAttachments = colorAttachments;
        description.pResolveAttachments = nullptr;
        description.pDepthStencilAttachment = depthStencilAttachment;
        description.preserveAttachmentCount = 0;
        description.pPreserveAttachments = nullptr;
        return description;
    }

    inline static VkSubpassDependency
    subpassDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
                      VkPipelineStageFlags dstStageMask,
                      VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDependencyFlags dependencyFlags) {
        VkSubpassDependency dependency;
        dependency.srcSubpass = srcSubpass;
        dependency.dstSubpass = dstSubpass;
        dependency.srcStageMask = srcStageMask;
        dependency.dstStageMask = dstStageMask;
        dependency.srcAccessMask = srcAccessMask;
        dependency.dstAccessMask = dstAccessMask;
        dependency.dependencyFlags = dependencyFlags;
        return dependency;
    }

    inline static VkRenderPassCreateInfo renderPassCreateInfo(
            uint32_t attachmentCount, VkAttachmentDescription *attachmentDescriptions,
            uint32_t dependencyCount, VkSubpassDependency *subpassDependencies,
            uint32_t subpassCount, VkSubpassDescription *subpassDescriptions
            ) {
        VkRenderPassCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.flags = 0;
        info.pNext = nullptr;
        info.attachmentCount = attachmentCount;
        info.pAttachments = attachmentDescriptions;
        info.dependencyCount = dependencyCount;
        info.pDependencies = subpassDependencies;
        info.subpassCount = subpassCount;
        info.pSubpasses = subpassDescriptions;
        return info;
    }

    inline struct VkShaderModuleCreateInfo shaderModuleCreateInfo(size_t codeSize, const uint32_t *code) {
        VkShaderModuleCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = codeSize;
        info.pCode = code;
        return info;
    }

    inline static VkFramebufferCreateInfo framebufferCreateInfo(VkRenderPass renderPass,
            uint32_t attachmentCount, VkImageView *attachments,
            uint32_t width, uint32_t height) {
        VkFramebufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.renderPass = renderPass;
        info.attachmentCount = attachmentCount;
        info.pAttachments = attachments;
        info.width = width;
        info.height = height;
        info.layers = 1;
        return info;
    }

    inline struct VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags usageFlags, const VkCommandBufferInheritanceInfo *inheritanceInfo =  nullptr) {
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = nullptr;
        info.flags = usageFlags;
        info.pInheritanceInfo = inheritanceInfo;
        return info;
    }

    inline struct VkImageSubresourceRange imageSubresourceRange(
            uint32_t levelCount, uint32_t baseMipLevel, uint32_t layerCount, uint32_t baseArrayLayer,
            VkImageAspectFlags aspectMask) {
        VkImageSubresourceRange range = {};
        range.levelCount = levelCount;
        range.baseMipLevel = baseMipLevel;
        range.layerCount = layerCount;
        range.baseArrayLayer = baseArrayLayer;
        range.aspectMask = aspectMask;
        return range;
    }

    inline struct VkImageMemoryBarrier imageMemoryBarrier(VkImage image,
            VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageSubresourceRange subresourceRange,
            VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout newLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED) {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = nullptr;
        barrier.image = image;
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstAccessMask = dstAccessMask;
        barrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
        barrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.subresourceRange = subresourceRange;
        return barrier;
    }

    inline static VkRenderPassBeginInfo renderPassBeginInfo(VkFramebuffer framebuffer, VkRenderPass renderPass, VkExtent2D extent,
            uint32_t clearValueCount, VkClearValue *clearValues, VkOffset2D offset = {0, 0}) {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext = nullptr;
        info.framebuffer = framebuffer;
        info.renderPass = renderPass;
        info.renderArea.extent = extent;
        info.renderArea.offset = offset;
        info.clearValueCount = clearValueCount;
        info.pClearValues = clearValues;
        return info;
    }

    inline static VkViewport viewport(uint32_t width, uint32_t height, uint32_t x, uint32_t y, float minDepth, float maxDepth) {
        VkViewport viewport = {};
        viewport.width = width;
        viewport.height = height;
        viewport.x = x;
        viewport.y = y;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
        return viewport;
    }

    inline static VkRect2D rect2D(VkOffset2D offset, VkExtent2D extent) {
        VkRect2D rect = {};
        rect.offset = offset;
        rect.extent = extent;
        return rect;
    }

    inline static VkRect2D rect2D(uint32_t x, uint32_t y, VkExtent2D extent) {
        VkRect2D rect = {};
        rect.offset.x = x;
        rect.offset.y = y;
        rect.extent = extent;
        return rect;
    }

    inline static VkSubmitInfo submitInfo(uint32_t commandBufferCount, VkCommandBuffer *commandBuffers,
            uint32_t signalSemaphoreCount, VkSemaphore *signalSemaphores,
            uint32_t waitSemaphoreCount, VkSemaphore *waitSemaphores,
            VkPipelineStageFlags *waitDstStageMask) {
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext = nullptr;
        info.commandBufferCount = commandBufferCount;
        info.pCommandBuffers = commandBuffers; // &this->cmdBuffers[imageIndex];
        info.signalSemaphoreCount = signalSemaphoreCount;
        info.pSignalSemaphores = signalSemaphores;
        info.waitSemaphoreCount = waitSemaphoreCount;
        info.pWaitSemaphores = waitSemaphores;
        info.pWaitDstStageMask = waitDstStageMask;
        return info;
    }

    inline static VkPresentInfoKHR presentInfoKhr(uint32_t swapchainCount, VkSwapchainKHR *swapchains, const uint32_t *imageIndices,
            uint32_t waitSemaphoreCount, VkSemaphore *waitSemaphores) {
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pNext = nullptr;
        info.swapchainCount = swapchainCount;
        info.pSwapchains = swapchains;
        info.waitSemaphoreCount = waitSemaphoreCount;
        info.pWaitSemaphores = waitSemaphores;
        info.pImageIndices = imageIndices;
        info.pResults = nullptr;
        return info;
    }

    inline static VkCommandBufferAllocateInfo commandBufferAllocateInfo(uint32_t commandBufferCount, VkCommandPool commandPool, VkCommandBufferLevel commandBufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
        VkCommandBufferAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.commandBufferCount = commandBufferCount;
        info.commandPool = commandPool;
        info.level = commandBufferLevel;
        return info;
    }

    inline static VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0) {
        VkSemaphoreCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        return info;
    }

    inline static VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags = 0) {
        VkFenceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        return info;
    }

    inline static VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0) {
        VkCommandPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.queueFamilyIndex = queueFamilyIndex;
        return info;
    }

    inline static VkMappedMemoryRange mappedMemoryRange(VkDeviceMemory memory, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
        VkMappedMemoryRange range = {};
        range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range.pNext = nullptr;
        range.size = size;
        range.offset = offset;
        range.memory = memory;
        return range;
    }

    inline static VkBufferCreateInfo bufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkBufferCreateFlags flags = 0) {
        VkBufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.size = size;
        info.usage = usageFlags;

        // Only used when we have concurrent sharing mode
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = nullptr;
        return info;
    }

    inline static VkImageCreateInfo imageCreateInfo(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlagBits imageUsageFlagBits) {
        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.format = format; // VK_FORMAT_R8G8B8A8_UNORM; //Todo: Fix format hardcode
        info.extent.width = width;
        info.extent.height = height;
        info.extent.depth = 1;
        info.arrayLayers = 1;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.mipLevels = 1;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = imageUsageFlagBits; // VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = nullptr;
        return info;
    }

    inline static VkMemoryAllocateInfo memoryAllocateInfo(VkDeviceSize allocationSize, uint32_t memoryTypeIndex) {
        VkMemoryAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.allocationSize = allocationSize;
        info.memoryTypeIndex = memoryTypeIndex;
        return info;
    }

    inline static VkImageViewCreateInfo imageViewCreateInfo(VkImage image, VkFormat format, VkImageAspectFlags aspectMask) {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.image = image;
        info.format = format; // VK_FORMAT_R8G8B8A8_UNORM; //Todo: Fix format hardcode
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.subresourceRange.aspectMask = aspectMask; // VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.layerCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseMipLevel = 0;
        return info;
    }

    inline static VkBufferImageCopy bufferImageCopy(VkExtent3D imageExtent) {
        VkBufferImageCopy copy = {};
        copy.bufferOffset = 0;
        copy.bufferImageHeight = 0;
        copy.bufferRowLength = 0;
        copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy.imageSubresource.layerCount = 1;
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.mipLevel = 0;
        copy.imageOffset = {0, 0, 0};
        copy.imageExtent = imageExtent;
        return copy;
    }

    inline static VkDescriptorPoolSize descriptorPoolSize(VkDescriptorType type, uint32_t count) {
        VkDescriptorPoolSize size;
        size.type = type;
        size.descriptorCount = count;
        return size;
    }

    inline static VkDescriptorPoolCreateInfo descriptorPoolCreateInfo(uint32_t maxSets, uint32_t poolSizeCount, VkDescriptorPoolSize *poolSizes) {
        VkDescriptorPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.maxSets = maxSets;
        info.poolSizeCount = poolSizeCount;
        info.pPoolSizes = poolSizes;
        return info;
    }

    inline static VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(VkDescriptorPool descriptorPool, uint32_t setCount, const VkDescriptorSetLayout *setLayouts) {
        VkDescriptorSetAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.descriptorPool = descriptorPool;
        info.descriptorSetCount = setCount;
        info.pSetLayouts = setLayouts;
        return info;
    }

    inline static VkBufferCopy bufferCopy(VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0) {
        VkBufferCopy copy = {};
        copy.size = size;
        copy.srcOffset = srcOffset;
        copy.dstOffset = dstOffset;
        return copy;
    }
}

#endif //DYNAMICLINK_VULKANSTRUCTURES_H
