//
// Created by jonat on 24/09/2020.
//

#include <iostream>
#include "PipelineBuilder.h"

int PipelineBuilder::buildPipeline(DeviceInfo device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo, VkShaderModule vertexShader, VkShaderModule fragmentShader, VkPipeline *pipeline) {
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[] = {
            VKSTRUCT::pipelineShaderStageCreateInfo(vertexShader, VK_SHADER_STAGE_VERTEX_BIT),
            VKSTRUCT::pipelineShaderStageCreateInfo(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT)
    };

    VkGraphicsPipelineCreateInfo createInfo = VKSTRUCT::graphicsPipelineCreateInfo(
            renderPass, &vertexInputStateCreateInfo, &this->assemblyStateCreateInfo, &this->viewportStateCreateInfo,
            &this->rasterizationStateCreateInfo, &this->multisampleStateCreateInfo, &this->colorBlendStateCreateInfo,
            pipelineLayout, &this->dynamicStateCreateInfo, 2, shaderStageCreateInfo,
            (this->depthStencilOn)?&this->depthStencilStateCreateInfo:nullptr );

    if (vkCreateGraphicsPipelines(device.logical, 0, 1, &createInfo, nullptr, pipeline) != VK_SUCCESS) {
        std::cout << "Failed to do so" << std::endl;
    }
    return 0;
}

PipelineBuilder::PipelineBuilder() :
    assemblyStateCreateInfo(VKSTRUCT::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)),
    viewportStateCreateInfo(VKSTRUCT::pipelineViewportStateCreateInfo(1, 1)),
    rasterizationStateCreateInfo(VKSTRUCT::pipelineRasterizationStateCreateInfo()),
    multisampleStateCreateInfo(VKSTRUCT::pipelineMultisampleStateCreateInfo()),
    colorBlendAttachmentState(VKSTRUCT::pipelineColorBlendAttachmentState()),
    colorBlendStateCreateInfo(VKSTRUCT::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachmentState)),
    dynamicStates {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR},
    dynamicStateCreateInfo(VKSTRUCT::pipelineDynamicStateCreateInfo(2, dynamicStates)),
    depthStencilOn(false) {
}
