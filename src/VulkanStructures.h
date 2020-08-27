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
            const VkPipelineShaderStageCreateInfo *shaderStageCreateInfo) {
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
        info.pDepthStencilState = nullptr;
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
    attachmentDescription(VkFormat format, VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED) {
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

    inline static VkAttachmentReference attachmentReference(uint32_t attachment, VkImageLayout layout) {
        VkAttachmentReference reference;
        reference.attachment = 0;
        reference.layout = layout;
        return reference;
    }

    inline static VkSubpassDescription subpassDescription(
            uint32_t inputAttachmentCount, VkAttachmentReference *inputAttachments,
            uint32_t colorAttachmentCount, VkAttachmentReference *colorAttachments) {
        VkSubpassDescription description;
        description.flags = 0;
        description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        description.inputAttachmentCount = inputAttachmentCount;
        description.pInputAttachments = inputAttachments;
        description.colorAttachmentCount = colorAttachmentCount;
        description.pColorAttachments = colorAttachments;
        description.pResolveAttachments = nullptr;
        description.pDepthStencilAttachment = nullptr;
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
}

#endif //DYNAMICLINK_VULKANSTRUCTURES_H
