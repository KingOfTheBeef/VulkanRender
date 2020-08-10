/************************************
/ Macros used to import Vulkan functions
************************************/

// Export level functions
#if defined VK_EXPORT_FUNCTION

VK_EXPORT_FUNCTION( vkGetInstanceProcAddr )
#undef VK_EXPORT_FUNCTION
#endif


// Global level functions
#ifdef VK_GLOBAL_FUNCTION
VK_GLOBAL_FUNCTION(vkEnumerateInstanceVersion)
VK_GLOBAL_FUNCTION(vkEnumerateInstanceExtensionProperties)
VK_GLOBAL_FUNCTION(vkEnumerateInstanceLayerProperties)
VK_GLOBAL_FUNCTION(vkCreateInstance)
#undef VK_GLOBAL_FUNCTION
#endif

// Instance level functions
#ifdef VK_INSTANCE_FUNCTION
VK_INSTANCE_FUNCTION( vkCreateDevice )
VK_INSTANCE_FUNCTION( vkEnumeratePhysicalDevices )
VK_INSTANCE_FUNCTION( vkDestroyInstance )
VK_INSTANCE_FUNCTION( vkGetPhysicalDeviceFeatures )
VK_INSTANCE_FUNCTION( vkGetPhysicalDeviceProperties )
VK_INSTANCE_FUNCTION( vkGetDeviceProcAddr )
VK_INSTANCE_FUNCTION( vkGetPhysicalDeviceQueueFamilyProperties )
VK_INSTANCE_FUNCTION( vkDestroySurfaceKHR )
VK_INSTANCE_FUNCTION( vkGetPhysicalDeviceSurfaceSupportKHR )
VK_INSTANCE_FUNCTION( vkGetPhysicalDeviceSurfaceCapabilitiesKHR )
VK_INSTANCE_FUNCTION( vkGetPhysicalDeviceSurfaceFormatsKHR )
VK_INSTANCE_FUNCTION( vkGetPhysicalDeviceSurfacePresentModesKHR )
VK_INSTANCE_FUNCTION( vkEnumerateDeviceExtensionProperties )
VK_INSTANCE_FUNCTION( vkGetPhysicalDeviceMemoryProperties )
#ifdef VK_USE_PLATFORM_WIN32_KHR
VK_INSTANCE_FUNCTION( vkCreateWin32SurfaceKHR )
#endif
#undef VK_INSTANCE_FUNCTION
#endif

#ifdef VK_DEVICE_FUNCTION
VK_DEVICE_FUNCTION( vkDestroyDevice )
VK_DEVICE_FUNCTION( vkGetDeviceQueue )
VK_DEVICE_FUNCTION( vkDeviceWaitIdle )

// Swapchain Functions -> Will throw error if device is not enabled with swapchain extension
VK_DEVICE_FUNCTION( vkCreateSwapchainKHR )
VK_DEVICE_FUNCTION( vkDestroySwapchainKHR )
VK_DEVICE_FUNCTION( vkGetSwapchainImagesKHR )
VK_DEVICE_FUNCTION( vkAcquireNextImageKHR )
VK_DEVICE_FUNCTION( vkQueuePresentKHR )

VK_DEVICE_FUNCTION( vkCreateCommandPool )
VK_DEVICE_FUNCTION( vkAllocateCommandBuffers )
VK_DEVICE_FUNCTION( vkBeginCommandBuffer )
VK_DEVICE_FUNCTION( vkEndCommandBuffer )
VK_DEVICE_FUNCTION( vkCmdPipelineBarrier )
VK_DEVICE_FUNCTION( vkCmdClearColorImage )

VK_DEVICE_FUNCTION( vkQueueSubmit )
VK_DEVICE_FUNCTION( vkCreateSemaphore )
VK_DEVICE_FUNCTION( vkFreeCommandBuffers )
VK_DEVICE_FUNCTION( vkDestroyCommandPool )
VK_DEVICE_FUNCTION( vkDestroySemaphore )
VK_DEVICE_FUNCTION( vkCreateImageView )
VK_DEVICE_FUNCTION( vkCreateFramebuffer )
VK_DEVICE_FUNCTION( vkCreateRenderPass )
VK_DEVICE_FUNCTION( vkCreateGraphicsPipelines )
VK_DEVICE_FUNCTION( vkCreateShaderModule )
VK_DEVICE_FUNCTION( vkCreatePipelineLayout )
VK_DEVICE_FUNCTION( vkDestroyShaderModule )
VK_DEVICE_FUNCTION( vkCmdBeginRenderPass )
VK_DEVICE_FUNCTION( vkCmdBindPipeline )
VK_DEVICE_FUNCTION( vkCmdDraw )
VK_DEVICE_FUNCTION( vkCmdEndRenderPass )
VK_DEVICE_FUNCTION( vkDestroyImageView )
VK_DEVICE_FUNCTION( vkDestroyPipelineLayout )
VK_DEVICE_FUNCTION( vkDestroyRenderPass )
VK_DEVICE_FUNCTION( vkDestroyPipeline )
VK_DEVICE_FUNCTION( vkDestroyFramebuffer )
VK_DEVICE_FUNCTION( vkCreateBuffer )
VK_DEVICE_FUNCTION( vkGetBufferMemoryRequirements )
VK_DEVICE_FUNCTION( vkAllocateMemory )
VK_DEVICE_FUNCTION( vkBindBufferMemory )
VK_DEVICE_FUNCTION( vkMapMemory )
VK_DEVICE_FUNCTION( vkFlushMappedMemoryRanges )
VK_DEVICE_FUNCTION( vkUnmapMemory )
VK_DEVICE_FUNCTION( vkCreateFence )
VK_DEVICE_FUNCTION( vkWaitForFences )
VK_DEVICE_FUNCTION( vkResetFences )
VK_DEVICE_FUNCTION( vkCmdSetViewport )
VK_DEVICE_FUNCTION( vkCmdSetScissor )
VK_DEVICE_FUNCTION( vkCmdBindVertexBuffers )
VK_DEVICE_FUNCTION( vkDestroyFence )
VK_DEVICE_FUNCTION( vkDestroyBuffer )
VK_DEVICE_FUNCTION( vkFreeMemory )
VK_DEVICE_FUNCTION( vkCmdCopyBuffer )
VK_DEVICE_FUNCTION( vkCreateImage )
VK_DEVICE_FUNCTION( vkGetImageMemoryRequirements )
VK_DEVICE_FUNCTION( vkCmdCopyBufferToImage )
VK_DEVICE_FUNCTION( vkCreateSampler )
VK_DEVICE_FUNCTION( vkCreateDescriptorSetLayout )
VK_DEVICE_FUNCTION( vkCreateDescriptorPool )
VK_DEVICE_FUNCTION( vkAllocateDescriptorSets )
VK_DEVICE_FUNCTION( vkUpdateDescriptorSets )
VK_DEVICE_FUNCTION( vkBindImageMemory )
VK_DEVICE_FUNCTION( vkCmdBindDescriptorSets )

#undef VK_DEVICE_FUNCTION
#endif // VK_DEVICE_FUNCTION
