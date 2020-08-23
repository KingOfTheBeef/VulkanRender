# VulkanRender
Repo for me learning how to do vulkan.

All it does right now is draw a ~~cube~~ with a meme on it.\
Well now its a square with a meme on it...

# TODO
Unify all staging buffer to destination operations\
Have a structure to hold all drawable data elements\
Look into aliasing within a single buffer, currently multiple buffers in a single memory object\
Add data driven input, currently using tempVertexData.h\
\
LOTS OF REFACTORING TO DO :((


# Bugs
Swapchain recreation issues when window length or width is set to zero. (By dragging to resize screen). \
Lack of synchronisation allows writing over staging buffer before its contents are copied to its destination, (current fix uses vkDeviceWaitIdle, but should use fine grained synchro.)\
Undefined behaviour due to removed pipeline barrier in func submitStagingBuffer