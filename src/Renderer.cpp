//
// Created by jonat on 05/07/2020.
//

#include <iostream>
#include "Renderer.h"
#include "FileReader.h"
#include "tempVertexData.h"

int Renderer::initRenderPass(VkDevice device, VkFormat format) {
  VkAttachmentDescription attachmentDescriptions[1];
  attachmentDescriptions[0].format          = format;
  attachmentDescriptions[0].flags           = 0;
  attachmentDescriptions[0].initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED; // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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

  VkSubpassDependency subpassDependencies[2];
  subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  subpassDependencies[0].dstSubpass = 0;
  subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT ;
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

void Renderer::clean(DeviceInfo device) {

  for (auto & virtualFrame : this->virtualFrames) {
    vkFreeCommandBuffers(device.logical, this->cmdPool, 1, &virtualFrame.cmdBuffer);
    vkDestroySemaphore(device.logical, virtualFrame.imageFinishProcessingSema, nullptr);
    vkDestroySemaphore(device.logical, virtualFrame.imageAvailableSema, nullptr);
    vkDestroyFence(device.logical, virtualFrame.fence, nullptr);
    if (virtualFrame.framebuffer != VK_NULL_HANDLE) {
      vkDestroyFramebuffer(device.logical, virtualFrame.framebuffer, nullptr);
    }
  }

  vkDestroyBuffer(device.logical, this->vertexBuffer, nullptr);
  vkFreeMemory(device.logical, this->deviceMemory, nullptr);
  vkDestroyCommandPool(device.logical, this->cmdPool, nullptr);
  vkDestroyRenderPass(device.logical, this->renderPass, nullptr);
  vkDestroyPipeline(device.logical, this->pipeline, nullptr);
}

int Renderer::initGraphicPipeline(DeviceInfo device) {

  VkShaderModule vertexShader, fragmentShader;
  initShaderModule(device.logical, "shaders/vert.spv", &vertexShader);
  initShaderModule (device.logical, "shaders/frag.spv", &fragmentShader);

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

  /* Vertex data arranged as =
   * {x, y, z, w, r, g, b, a}
   * i.e. 4 32bit floats for xyzw coordinates and then rgba color values, interleaved
   * */

  VkVertexInputBindingDescription vertexInputBindingDescription[1];
  vertexInputBindingDescription[0].binding = 0;
  vertexInputBindingDescription[0].stride = Data::stride;// sizeof(float) * 8; //Todo: hardcoding
  vertexInputBindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  VkVertexInputAttributeDescription vertexInputAttributeDescription[2];
  // Position inputs
  vertexInputAttributeDescription[0].binding = 0;
  vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32A32_SFLOAT; // Hard coded RGBA format for vertex buffers
  vertexInputAttributeDescription[0].location = 0;
  vertexInputAttributeDescription[0].offset = Data::positionOffset;

  // Color inputs
  vertexInputAttributeDescription[1].binding = 0;
  vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
  vertexInputAttributeDescription[1].location = 1;
  vertexInputAttributeDescription[1].offset = Data::colorOffset; // sizeof(float) * 4; //Todo: Fix this hardcoding

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.pNext = nullptr;
  vertexInputStateCreateInfo.flags = 0;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
  vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescription;
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
  vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription;

  VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo = {};
  assemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  assemblyStateCreateInfo.pNext = nullptr;
  assemblyStateCreateInfo.flags = 0;
  assemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
  assemblyStateCreateInfo.topology = Data::topology; // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
  viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.pNext = nullptr;
  viewportStateCreateInfo.flags = 0;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = nullptr; // &viewport;
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = nullptr; // &scissorTest;

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

  VkDynamicState dynamicStates[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
  dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCreateInfo.pNext = nullptr;
  dynamicStateCreateInfo.flags = 0;
  dynamicStateCreateInfo.dynamicStateCount = 2;
  dynamicStateCreateInfo.pDynamicStates = dynamicStates;

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
  createInfo.pDynamicState = &dynamicStateCreateInfo;
  createInfo.pTessellationState = nullptr;
  createInfo.stageCount = 2;
  createInfo.pStages = shaderStageCreateInfo;
  createInfo.subpass = 0;

  if (vkCreateGraphicsPipelines(device.logical, 0, 1, &createInfo, nullptr, &this->pipeline) != VK_SUCCESS) {
    std::cout << "Failed to do so" << std::endl;
  }

  vkDestroyShaderModule(device.logical, vertexShader, nullptr);
  vkDestroyShaderModule(device.logical, fragmentShader, nullptr);
  vkDestroyPipelineLayout(device.logical, layout, nullptr);
  return 0;
}

int Renderer::initVertexBuffer(DeviceInfo device) {
  VkBufferCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.size = Data::vertexDataSize;

  createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.queueFamilyIndexCount = 0;       // Only used when we have concurrent sharing mode
  createInfo.pQueueFamilyIndices = nullptr;

  vkCreateBuffer(device.logical, &createInfo, nullptr, &this->vertexBuffer);


  VkMemoryRequirements memoryRequirements;
  VkPhysicalDeviceMemoryProperties memoryProperties;
  vkGetBufferMemoryRequirements(device.logical, this->vertexBuffer, &memoryRequirements);
  vkGetPhysicalDeviceMemoryProperties(device.physical, &memoryProperties);

  uint32_t memeoryIndex = -1;
  for (int i = 0; i < memoryProperties.memoryTypeCount;  i++) {
    if (memoryRequirements.memoryTypeBits & (1 << i)
     && memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
      memeoryIndex = i;
      break;
    }
  }
  if (memeoryIndex == -1) {
    std::cout << "Didn't find memory for buffer allocation" << std::endl;
  }

  VkMemoryAllocateInfo memoryAllocateInfo = {};
  memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memoryAllocateInfo.pNext = nullptr;
  memoryAllocateInfo.allocationSize = memoryRequirements.size;
  memoryAllocateInfo.memoryTypeIndex = memeoryIndex;

  vkAllocateMemory(device.logical, &memoryAllocateInfo, nullptr, &this->deviceMemory);

  vkBindBufferMemory(device.logical, this->vertexBuffer, this->deviceMemory, 0);

  void *ptrBuffer;
  vkMapMemory(device.logical, this->deviceMemory, 0, Data::vertexDataSize, 0, &ptrBuffer);
  memcpy(ptrBuffer, Data::vertexData, Data::vertexDataSize);


  VkMappedMemoryRange memoryRange = {};
  memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  memoryRange.pNext = nullptr;
  memoryRange.size = VK_WHOLE_SIZE;
  memoryRange.offset = 0;
  memoryRange.memory = this->deviceMemory;

  vkFlushMappedMemoryRanges(device.logical, 1, &memoryRange);
  vkUnmapMemory(device.logical, this->deviceMemory);

  return 0;
}

int Renderer::prepareVirtualFrame(DeviceInfo device, VirtualFrame *virtualFrame, VkExtent2D extent, VkImageView *imageView, VkImage image) {
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

    vkCmdPipelineBarrier(virtualFrame->cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
  }

  VkClearValue clearColor = { 1.0f, 0.8f, 0.4f, 0.0f };
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
  vkCmdBindVertexBuffers(virtualFrame->cmdBuffer, 0, 1, &this->vertexBuffer, &offset);

  vkCmdDraw(virtualFrame->cmdBuffer, Data::vertexCount, 1, 0, 0);

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

    vkCmdPipelineBarrier(virtualFrame->cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
  }

  vkEndCommandBuffer(virtualFrame->cmdBuffer);

  return 0;
}

int Renderer::draw(DeviceInfo device, SwapchainInfo swapchain) {
  static uint32_t virtualFrameIndex = 0;
  virtualFrameIndex = (virtualFrameIndex + 1) % this->virtualFrameCount;
  uint32_t imageIndex = 0;
  vkWaitForFences(device.logical, 1, &this->virtualFrames[virtualFrameIndex].fence, VK_TRUE, UINT64_MAX);
  vkResetFences(device.logical, 1, &this->virtualFrames[virtualFrameIndex].fence);
  VkResult result = vkAcquireNextImageKHR(device.logical, swapchain.swapchain, UINT64_MAX, this->virtualFrames[virtualFrameIndex].imageAvailableSema, VK_NULL_HANDLE, &imageIndex);
  switch (result) {
    case VK_SUCCESS:
    case VK_SUBOPTIMAL_KHR:
      break;
    case VK_ERROR_OUT_OF_DATE_KHR:
      return 1;
    default:
      return 2;
  }

  prepareVirtualFrame(device, &this->virtualFrames[virtualFrameIndex], swapchain.extent, &swapchain.imageViews[imageIndex], swapchain.images[imageIndex]);

  VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.pNext = nullptr;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &this->virtualFrames[virtualFrameIndex].cmdBuffer; // &this->cmdBuffers[imageIndex];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &this->virtualFrames[virtualFrameIndex].imageFinishProcessingSema;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &this->virtualFrames[virtualFrameIndex].imageAvailableSema;
  submitInfo.pWaitDstStageMask = &stageFlags;

  vkQueueSubmit(device.graphicQueue, 1, &submitInfo, this->virtualFrames[virtualFrameIndex].fence);

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pNext = nullptr;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain.swapchain;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &this->virtualFrames[virtualFrameIndex].imageFinishProcessingSema;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  result = vkQueuePresentKHR(device.displayQueue, &presentInfo);

  switch (result) {
    case VK_SUCCESS:
    case VK_SUBOPTIMAL_KHR:
      break;
    case VK_ERROR_OUT_OF_DATE_KHR:
      return 1;
    default:
      return 2;
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
    vkCreateSemaphore(device.logical, &semaphoreCreateInfo, nullptr, &this->virtualFrames[i].imageFinishProcessingSema);
    vkCreateSemaphore(device.logical, &semaphoreCreateInfo, nullptr, &this->virtualFrames[i].imageAvailableSema);
    vkCreateFence(device.logical, &fenceCreateInfo, nullptr, &this->virtualFrames[i].fence);
  }

  return 0;
}

int Renderer::initCommandPool(DeviceInfo device) {
  VkCommandPoolCreateInfo commandPoolCreateInfo = {};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.pNext = nullptr;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
  commandPoolCreateInfo.queueFamilyIndex = device.graphicQueueIndex;
  vkCreateCommandPool(device.logical, &commandPoolCreateInfo, nullptr, &this->cmdPool);
  return 0;
}

int Renderer::initRenderer(DeviceInfo device, VkFormat format) {
  this->initRenderPass(device.logical, format);
  this->initGraphicPipeline(device);
  this->initVertexBuffer(device);
  this->initCommandPool(device);
  this->initVirtualFrames(device);
  return 0;
}
