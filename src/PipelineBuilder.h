//
// Created by jonat on 24/09/2020.
//

#ifndef DYNAMICLINK_PIPELINEBUILDER_H
#define DYNAMICLINK_PIPELINEBUILDER_H

#include "VulkanInit.h"
#include <vulkan/vulkan.h>
#include "Structures.h"
#include "VulkanStructures.h"

class PipelineBuilder {
private:;
    VkPipelineInputAssemblyStateCreateInfo  assemblyStateCreateInfo;
    VkPipelineViewportStateCreateInfo       viewportStateCreateInfo;
    VkPipelineRasterizationStateCreateInfo  rasterizationStateCreateInfo;
    VkPipelineMultisampleStateCreateInfo    multisampleStateCreateInfo;
    VkPipelineColorBlendAttachmentState     colorBlendAttachmentState;
    VkPipelineColorBlendStateCreateInfo     colorBlendStateCreateInfo;
    VkDynamicState dynamicStates[2];
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;

public:
    static int createPipelineLayout(DeviceInfo device, uint32_t descriptorSetCount, VkDescriptorSetLayout *descriptorSetLayouts, VkPipelineLayout *pipelineLayout) {
        VkPipelineLayoutCreateInfo layoutCreateInfo = VKSTRUCT::pipelineLayoutCreateInfo(descriptorSetCount, descriptorSetLayouts);
        return vkCreatePipelineLayout(device.logical, &layoutCreateInfo, nullptr, pipelineLayout);
    }

    PipelineBuilder();
    int buildPipeline(DeviceInfo device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo, VkShaderModule vertexShader, VkShaderModule fragmentShader, VkPipeline *pipeline);
};


#endif //DYNAMICLINK_PIPELINEBUILDER_H
