#include <iostream>

#include "VulkanInit.h"

#define VULKAN_DLL "vulkan-1.dll"

#define VK_EXPORT_FUNCTION( func ) PFN_##func func;
#define VK_GLOBAL_FUNCTION( func ) PFN_##func func;
#define VK_INSTANCE_FUNCTION( func ) PFN_##func func;
#define VK_DEVICE_FUNCTION( func ) PFN_##func func;

#include "VulkanFunctionList.inl"

void LoadVulkanLibrary(VKLIB *VulkanLib) {
    *VulkanLib = LoadLibrary(VULKAN_DLL);
    if(*VulkanLib == nullptr) {
        std::cout << "Failure: Unable to load Vulkan lib" << std::endl;
    }
}

void LoadVulkanSystemFunctions(VKLIB vkLib) {

    #define VK_EXPORT_FUNCTION( func ) \
    func = reinterpret_cast<PFN_##func>(GetProcAddress(vkLib, #func));  \
    if (func == nullptr) {                                                  \
        std::cout << "Unable to find function: " << #func << std::endl;     \
    } else {                                                                \
        std::cout << "Successfully loaded function: " << #func << std::endl;\
    }

    #define VK_GLOBAL_FUNCTION( func ) \
    func = reinterpret_cast<PFN_##func>(vkGetInstanceProcAddr(nullptr, #func));  \
    if (func == nullptr) {                                                  \
        std::cout << "Unable to find function: " << #func << std::endl;     \
    } else {                                                                \
        std::cout << "Successfully loaded function: " << #func << std::endl;\
    }

    #include "VulkanFunctionList.inl"
}

void LoadVulkanInstanceFunctions(VkInstance instance) {
    #define VK_INSTANCE_FUNCTION( func ) \
    func = reinterpret_cast<PFN_##func>(vkGetInstanceProcAddr(instance, #func));  \
    if (func == nullptr) {                                                  \
        std::cout << "Unable to find function: " << #func << std::endl;     \
    } else {                                                                \
        std::cout << "Successfully loaded function: " << #func << std::endl;\
    }

    #include "VulkanFunctionList.inl"
}

void LoadVulkanDeviceFunctions(VkDevice device) {
    #define VK_DEVICE_FUNCTION( func ) \
    func = reinterpret_cast<PFN_##func>(vkGetDeviceProcAddr(device, #func));  \
    if (func == nullptr) {                                                  \
        std::cout << "Unable to find function: " << #func << std::endl;     \
    } else {                                                                \
        std::cout << "Successfully loaded function: " << #func << std::endl;\
    }

    #include "VulkanFunctionList.inl"
}

