#pragma once

#if AX_RENDERER_VK

#include <vulkan/vulkan.h>
#if AX_NATIVE_UI_WIN32
	#include "AxCore/Platform/AX_OS_WINDOW.h"
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
//	E(vkCmdSetCheckpointNV)	\
//	E(vkGetQueueCheckpointDataNV) \
//	E(vkCreateSwapchainKHR) \
//------


#endif
