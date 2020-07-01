#include <Windows.h>

#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"

#define VK_EXPORT_FUNCTION( func ) extern PFN_##func func;
#define VK_GLOBAL_FUNCTION( func ) extern PFN_##func func;
#define VK_INSTANCE_FUNCTION( func ) extern PFN_##func func;
#define VK_DEVICE_FUNCTION( func ) extern PFN_##func func;

#include "VulkanFunctionList.inl"

typedef HINSTANCE VKLIB;

void LoadVulkanLibrary(VKLIB *VulkanLib);
void LoadVulkanSystemFunctions(VKLIB VulkanLib);
void LoadVulkanInstanceFunctions(VkInstance instance);
void LoadVulkanDeviceFunctions(VkDevice device);
