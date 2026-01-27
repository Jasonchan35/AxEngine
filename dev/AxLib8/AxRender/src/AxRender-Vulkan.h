#pragma once

#if AX_RENDERER_VK

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <VulkanMemoryAllocator/vk_mem_alloc.h>

#if AX_NATIVE_UI_WIN32
	#include "AxCore-OS_Windows.h"
	#include <vulkan/vulkan_win32.h>
#endif

#define AX_VkExtProcList_ENUM_LIST(E) \
	E(vkCreateDebugReportCallbackEXT) \
	E(vkDestroyDebugReportCallbackEXT) \
	E(vkSetDebugUtilsObjectNameEXT) \
	E(vkSetDebugUtilsObjectTagEXT) \
	E(vkQueueInsertDebugUtilsLabelEXT) \
	E(vkCmdBeginDebugUtilsLabelEXT) \
	E(vkCmdEndDebugUtilsLabelEXT) \
	E(vkCmdDrawMeshTasksEXT) \
//	E(vkCmdSetCheckpointNV)	\
//	E(vkGetQueueCheckpointDataNV) \
//	E(vkCreateSwapchainKHR) \
//------


#endif
