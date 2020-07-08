//
// Created by jonat on 05/07/2020.
//

#include <iostream>
#include "Renderer.h"
#include "FileReader.h"

int Renderer::initRenderPass(VkDevice device, VkFormat format) {
  VkAttachmentDescription attachmentDescriptions[1];
  attachmentDescriptions[0].format          = format;
  attachmentDescriptions[0].flags           = 0;
  attachmentDescriptions[0].initialLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  attachmentDescriptions[0].finalLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  attachmentDescriptions[0].loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachmentDescriptions[0].storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
  attachmentDescriptions[0].stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachmentDescriptions[0].stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachmentDescriptions[0].samples         = VK_SAMPLE_COUNT_1_BIT;

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

  VkRenderPassCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pNext = nullptr;
  createInfo.attachmentCount = 1;
  createInfo.pAttachments = attachmentDescriptions;
  createInfo.dependencyCount = 0;
  createInfo.pDependencies = nullptr;
  createInfo.subpassCount = 1;
  createInfo.pSubpasses = subpassDescriptions;

  if (vkCreateRenderPass(device, &createInfo, nullptr, &this->renderPass) != VK_SUCCESS) {
    std::cout << "Failed to create render pass" << std::endl;
    return -1;
  }
  return 0;
}

int Renderer::initFramebuffers(VkDevice device, uint32_t imageViewCount, VkImageView *imageViews) {
  this->framebuffers = new VkFramebuffer[imageViewCount];

  for (int i = 0; i < imageViewCount; i++) {

    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.pNext = nullptr;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &imageViews[i];
    createInfo.renderPass = this->renderPass;
    createInfo.width = 500;
    createInfo.height = 500;
    createInfo.layers = 1;

    vkCreateFramebuffer(device, &createInfo, nullptr, &this->framebuffers[i]);
  }

  return 0;
}

int Renderer::initShaderModules(VkDevice device, const char* filename, VkShaderModule *shaderModule) {
  BinaryFile prog;
  if (FileReader::readFileBin(filename, &prog)) {
    std::cout << "Unable to open file " << filename << std::endl;
  }

  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.codeSize = prog.length;
  createInfo.pCode = reinterpret_cast<uint32_t*>(prog.data);

  VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, shaderModule);
  FileReader::freeFileBin(&prog);
  return result != VK_SUCCESS;
}

void Renderer::clean() {
  delete(this->framebuffers);
}

int Renderer::initGraphicPipeline(VkDevice device) {
  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.pNext = nullptr;
  vertexInputStateCreateInfo.flags = 0;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
  vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
  vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

  VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo = {};
  assemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  assemblyStateCreateInfo.pNext = nullptr;
  assemblyStateCreateInfo.flags = 0;
  assemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
  assemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkViewport viewport = {};
  viewport.x = 0;
  viewport.y = 0;
  viewport.width = 300;
  viewport.height = 300;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissorTest = {};
  scissorTest.offset.x = 0;
  scissorTest.offset.y = 0;
  scissorTest.extent.height = 300;
  scissorTest.extent.width = 300;

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
  viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.pNext = nullptr;
  viewportStateCreateInfo.flags = 0;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = &viewport;
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = &scissorTest;

  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
  rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationStateCreateInfo.pNext  = nullptr;
  rasterizationStateCreateInfo.flags = 0;
  rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
  rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
  rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
  rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
  rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
  rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
  rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
  rasterizationStateCreateInfo.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
  multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleStateCreateInfo.pNext = nullptr;
  multisampleStateCreateInfo.flags = 0;
  multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
  multisampleStateCreateInfo.minSampleShading = 1.0f;
  multisampleStateCreateInfo.pSampleMask = nullptr;
  multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
  multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
  colorBlendAttachmentState.blendEnable = VK_FALSE;
  colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachmentState.colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
          | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
  colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCreateInfo.pNext = nullptr;
  colorBlendStateCreateInfo.flags = 0;
  colorBlendStateCreateInfo.attachmentCount = 1;
  colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
  colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
  colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
  colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
  colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

  VkGraphicsPipelineCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.renderPass = this->renderPass;
  createInfo.layout = {};
  createInfo.basePipelineHandle = VK_NULL_HANDLE;
  createInfo.basePipelineIndex = -1;
  createInfo.pVertexInputState = &vertexInputStateCreateInfo;
  createInfo.pInputAssemblyState = &assemblyStateCreateInfo;
  createInfo.pViewportState = &viewportStateCreateInfo;
  createInfo.pRasterizationState = &rasterizationStateCreateInfo;
  createInfo.pMultisampleState = &multisampleStateCreateInfo;
  createInfo.pColorBlendState = &colorBlendStateCreateInfo;
  vkCreateGraphicsPipelines(device, nullptr, 1,  )
  return 0;
}
