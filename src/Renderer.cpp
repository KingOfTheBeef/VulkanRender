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
