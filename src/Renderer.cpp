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

int Renderer::initShaderModule(VkDevice device, const char* filename, VkShaderModule *shaderModule) {
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

int Renderer::initGraphicPipeline(DeviceInfo device) {

  VkShaderModule vertexShader, fragmentShader;
  initShaderModule(device.logical, "shader/vert.spv", &vertexShader);
  initShaderModule (device.logical, "shader/frag.spv", &fragmentShader);

  VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2];
  shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageCreateInfo[0].pNext = nullptr;
  shaderStageCreateInfo[0].flags = 0;
  shaderStageCreateInfo[0].module = vertexShader;
  shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStageCreateInfo[0].pName = "main";
  shaderStageCreateInfo[0].pSpecializationInfo = nullptr;

  shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageCreateInfo[1].pNext = nullptr;
  shaderStageCreateInfo[1].flags = 0;
  shaderStageCreateInfo[1].module = fragmentShader;
  shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStageCreateInfo[1].pName = "main";
  shaderStageCreateInfo[1].pSpecializationInfo = nullptr;


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

  VkPipelineLayoutCreateInfo layoutCreateInfo = {};
  layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layoutCreateInfo.pNext = nullptr;
  layoutCreateInfo.flags = 0;
  layoutCreateInfo.setLayoutCount = 0;
  layoutCreateInfo.pSetLayouts = nullptr;
  layoutCreateInfo.pushConstantRangeCount = 0;
  layoutCreateInfo.pPushConstantRanges = nullptr;

  VkPipelineLayout layout;
  vkCreatePipelineLayout(device.logical, &layoutCreateInfo, nullptr, &layout);

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
  createInfo.layout = layout;
  createInfo.pDepthStencilState = nullptr;
  createInfo.pDynamicState = nullptr;
  createInfo.pTessellationState = nullptr;
  createInfo.stageCount = 2;
  createInfo.pStages = shaderStageCreateInfo;
  createInfo.subpass = 0;

  vkCreateGraphicsPipelines(device.logical, 0, 1, &createInfo, nullptr, &this->pipeline);

  vkDestroyShaderModule(device.logical, vertexShader, nullptr);
  vkDestroyShaderModule(device.logical, fragmentShader, nullptr);
  return 0;
}

int Renderer::initCommandBuffers(DeviceInfo device, uint32_t bufferCount) {
  if (bufferCount < 1) {
    std::cout << "Buffer count less than 1!" << std::endl;
    return -1;
  }

  VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
  cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmdPoolCreateInfo.pNext = nullptr;
  cmdPoolCreateInfo.flags = 0;
  cmdPoolCreateInfo.queueFamilyIndex = device.graphicQueueIndex;

  if (vkCreateCommandPool(device.logical, &cmdPoolCreateInfo, nullptr, &this->cmdPool) != VK_SUCCESS) {
    std::cout << "Failure to create command pool" << std::endl;
  }

  // vkGetSwapchainImagesKHR(this->deviceInfo.logical, this->swapchainInfo.swapchain, &this->cmdBufferCount, nullptr);

  this->cmdBufferCount = bufferCount;

  this->cmdBuffers = new VkCommandBuffer[this->cmdBufferCount];

  VkCommandBufferAllocateInfo allocateInfo = {};
  allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.pNext = nullptr;
  allocateInfo.commandPool = this->cmdPool;
  allocateInfo.commandBufferCount = this->cmdBufferCount;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  if (vkAllocateCommandBuffers(device.logical, &allocateInfo, this->cmdBuffers) != VK_SUCCESS) {
    std::cout << "Failed to allocate command buffers" << std::endl;
    return -1;
  }
  return 0;
}

int Renderer::recordCommandBuffers(DeviceInfo device, VkImage *images) {
  VkCommandBufferBeginInfo commandBufferBeginInfo = {};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  VkClearValue clearColor;
  clearColor.color = {{1.0f, 0.4f, 0.4f, 0.0f}};

  VkImageSubresourceRange subresourceRange = {};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;

  for (int i = 0; i < this->cmdBufferCount; i++) {
    vkBeginCommandBuffer(this->cmdBuffers[i], &commandBufferBeginInfo);

    if (device.graphicQueueIndex != device.displayQueueIndex) {
      VkImageMemoryBarrier preClearMemBarrier = {};
      preClearMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      preClearMemBarrier.pNext = nullptr;
      preClearMemBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      preClearMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      preClearMemBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      preClearMemBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      preClearMemBarrier.srcQueueFamilyIndex = device.displayQueueIndex;
      preClearMemBarrier.dstQueueFamilyIndex = device.graphicQueueIndex;
      preClearMemBarrier.image = images[i];
      preClearMemBarrier.subresourceRange = subresourceRange;

      vkCmdPipelineBarrier(this->cmdBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &preClearMemBarrier);
    }

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = this->renderPass;
    renderPassBeginInfo.framebuffer = this->framebuffers[i];
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;
    renderPassBeginInfo.renderArea.offset = {0,0};
    renderPassBeginInfo.renderArea.extent = {500, 500};

    vkCmdBeginRenderPass(this->cmdBuffers[i], &renderPassBeginInfo,  VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(this->cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);

    vkCmdDraw(this->cmdBuffers[i], 3, 1, 0, 0);


    if (device.graphicQueueIndex != device.displayQueueIndex) {
      VkImageMemoryBarrier postClearMemBarrier = {};
      postClearMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      postClearMemBarrier.pNext = nullptr;
      postClearMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      postClearMemBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      postClearMemBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      postClearMemBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      postClearMemBarrier.srcQueueFamilyIndex = device.graphicQueueIndex;
      postClearMemBarrier.dstQueueFamilyIndex = device.displayQueueIndex;
      postClearMemBarrier.image = images[i];
      postClearMemBarrier.subresourceRange = subresourceRange;

      vkCmdPipelineBarrier(this->cmdBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &postClearMemBarrier);
    }

    if (vkEndCommandBuffer(this->cmdBuffers[i]) != VK_SUCCESS) {
      std::cout << "Failure to record cmd buffer" << std::endl;
      return -1;
    }
  }
  return 0;
}
