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
VK_INSTANCE_FUNCTION(vkCreateDevice)
VK_INSTANCE_FUNCTION(vkEnumeratePhysicalDevices)
VK_INSTANCE_FUNCTION(vkDestroyInstance)
VK_INSTANCE_FUNCTION(vkGetPhysicalDeviceFeatures)
VK_INSTANCE_FUNCTION(vkGetPhysicalDeviceProperties)
VK_INSTANCE_FUNCTION(vkGetDeviceProcAddr)
VK_INSTANCE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)
VK_INSTANCE_FUNCTION(vkDestroySurfaceKHR)
VK_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR)
VK_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
VK_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR)
VK_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR)
VK_INSTANCE_FUNCTION(vkEnumerateDeviceExtensionProperties)
#ifdef VK_USE_PLATFORM_WIN32_KHR
VK_INSTANCE_FUNCTION(vkCreateWin32SurfaceKHR)
#endif
#undef VK_INSTANCE_FUNCTION
#endif

#ifdef VK_DEVICE_FUNCTION
VK_DEVICE_FUNCTION(vkDestroyDevice)
VK_DEVICE_FUNCTION(vkGetDeviceQueue)
VK_DEVICE_FUNCTION(vkDeviceWaitIdle)

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
#undef VK_DEVICE_FUNCTION
#endif // VK_DEVICE_FUNCTION
