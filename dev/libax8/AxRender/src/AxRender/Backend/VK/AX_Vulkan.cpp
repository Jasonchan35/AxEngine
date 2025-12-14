module;

module AxRender;

#if AX_RENDERER_VK
import :AX_Vulkan;
import :Renderer_VK;

namespace ax::AxRender {

AX_VkInstanceCreateInfo::Props::Props() {
	VkResult err;
	uint32_t count = 0;
	{
		err = vkEnumerateInstanceLayerProperties(&count, nullptr);
		AX_VkUtil::throwIfError(err);

		layers.resize(count);
		err = vkEnumerateInstanceLayerProperties(&count, layers.data());
		AX_VkUtil::throwIfError(err);
	}

	{
		err = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
		AX_VkUtil::throwIfError(err);

		extensions.resize(count);
		err = vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
		AX_VkUtil::throwIfError(err);
	}
}

bool AX_VkInstanceCreateInfo::Props::hasLayer(StrView name) const {
	for (auto& e : layers) {
		if (StrView_c_str(e.layerName) == name)
			return true;
	}
	return false;
}

bool AX_VkInstanceCreateInfo::Props::hasExtension(StrView name) const {
	for (auto& e : extensions) {
		if (StrView_c_str(e.extensionName) == name)
			return true;
	}
	return false;
}

bool AX_VkInstanceCreateInfo::enableLayer(StrLit name) {
	if (!props.hasLayer(name))
		return false;
	_enabledLayerNames.append(name.c_str());
	return true;
}

bool AX_VkInstanceCreateInfo::enableExtension(StrLit name) {
	if (!props.hasExtension(name))
		return false;
	_enabledExtensionNames.append(name.c_str());
	return true;
}

void AX_VkInstanceCreateInfo::logInfo() {
	TempString s = "== AX_VkInstanceCreateInfo::logInfo ==";
	
#if 0
	s.append("VkInstance Layers\n");
	for (auto& e : props.layers) {
		FmtTo(s, "  {:40} spec={}, impl={:3}, desc={}\n",
			StrView_c_str(e.layerName),
			e.specVersion,
			e.implementationVersion,
			StrView_c_str(e.description));
	}

	s.append("VkInstance Extensions\n");
	for (auto& e : props.extensions) {
		FmtTo(s, "  {:40} spec={}\n",
			StrView_c_str(e.extensionName),
			e.specVersion);
	}
#endif

	AX_LOG("{}", s);
}

 AX_VkApplicationInfo::AX_VkApplicationInfo() {
	sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	pNext = nullptr;
	pApplicationName = "Renderer_VK";
	applicationVersion = 0;
	pEngineName = "Renderer_VK";
	engineVersion = 0;
	apiVersion = AX_VkUtil::makeApiVersion(0, 1, 3, 0);
//	apiVersion = VK_API_VERSION_1_3;
 }

AX_VkInstanceCreateInfo::AX_VkInstanceCreateInfo() {
	sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	pNext = nullptr;
	flags = 0;
	pApplicationInfo = &appInfo;
	enabledLayerCount = 0;
	ppEnabledLayerNames = nullptr;
	enabledExtensionCount = 0;
	ppEnabledExtensionNames = nullptr;
}

void AX_VkSurfaceKHR::destroy() {
	if (_surface) {
		vkDestroySurfaceKHR(_dev->inst(), _surface, AX_VkUtil::allocCallbacks());
		_surface = VK_NULL_HANDLE;
		_dev = nullptr;

		_supportedFormats.clear();
		_supportedPresentModes.clear();

		_graphQueueFamilyIndex   = AX_VkQueueFamilyIndex::Invalid;
		_presentQueueFamilyIndex = AX_VkQueueFamilyIndex::Invalid;
	}
}

#if AX_NATIVE_UI_WIN32
void AX_VkSurfaceKHR::create_Win32(AX_VkDevice& dev, HINSTANCE hInstance, HWND hwnd) {
	destroy();
	_dev = &dev;

	VkWin32SurfaceCreateInfoKHR ci = {};
	ci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	ci.pNext = nullptr;
	ci.flags = 0;
	ci.hinstance = hInstance;
	ci.hwnd = hwnd;

	auto err = vkCreateWin32SurfaceKHR(_dev->inst(), &ci, AX_VkUtil::allocCallbacks(), &_surface);
	AX_VkUtil::throwIfError(err);

	_onCreate();
}
#endif

void AX_VkSurfaceKHR::_onCreate() {
	VkResult err;
	auto phyDev = _dev->physicalDevice();

	{
		uint32_t count;
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(*phyDev, _surface, &count, nullptr);
		AX_VkUtil::throwIfError(err);

		_supportedFormats.resize(count);
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(*phyDev, _surface, &count, _supportedFormats.data());
		AX_VkUtil::throwIfError(err);
	}
	{
		uint32_t count;
		err = vkGetPhysicalDeviceSurfacePresentModesKHR(*phyDev, _surface, &count, nullptr);
		AX_VkUtil::throwIfError(err);

		_supportedPresentModes.resize(count);
		err = vkGetPhysicalDeviceSurfacePresentModesKHR(*phyDev, _surface, &count, _supportedPresentModes.data());
		AX_VkUtil::throwIfError(err);
	}

	{
		_graphQueueFamilyIndex   = AX_VkQueueFamilyIndex::Invalid;
		_presentQueueFamilyIndex = AX_VkQueueFamilyIndex::Invalid;

		_graphQueueFamilyIndex = _dev->graphQueueFamilyIndex();

		if (checkQueueFamilySupportPresent(_graphQueueFamilyIndex)) {
			_presentQueueFamilyIndex = _graphQueueFamilyIndex; // graphQueue can do preset too

		} else if (findQueueFamilySupportPresent(_presentQueueFamilyIndex)) {
			throw Error_Undefined("Vulkan cannot find queue support present");
		}
	}
}

bool AX_VkSurfaceKHR::checkQueueFamilySupportPresent(AX_VkQueueFamilyIndex queueFamilyIndex) {
	// physical device supports presentation to a given surface
	VkBool32 b = false;
	auto err = vkGetPhysicalDeviceSurfaceSupportKHR(*_dev, ax_enum_int(queueFamilyIndex), _surface, &b);
	AX_VkUtil::throwIfError(err);
	return b != 0;
}

bool AX_VkSurfaceKHR::findQueueFamilySupportPresent(AX_VkQueueFamilyIndex& outIndex) {
	uint32_t n = AX_VkUtil::castUInt32(_dev->physicalDevice()->queueFamilyProps().size());

	for (uint32_t i = 0; i < n; i++) {
		auto index = static_cast<AX_VkQueueFamilyIndex>(i);
		if (checkQueueFamilySupportPresent(index)) {
			outIndex = index;
			return true;
		}
	}
	return false;
}

VkSurfaceCapabilitiesKHR AX_VkSurfaceKHR::getCapabilities() {
	VkSurfaceCapabilitiesKHR o = {};
	auto err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*_dev, _surface, &o);
	AX_VkUtil::throwIfError(err);
	return o;
}

bool AX_VkSurfaceKHR::isSupportPresentMode(VkPresentModeKHR presentMode) {
	for (auto& e : _supportedPresentModes) {
		if (e == presentMode) {
			return true;
		}
	}
	return false;
}

bool AX_VkSurfaceKHR::isSupportFormat(VkFormat format, VkColorSpaceKHR colorSpace) {
	for (auto& e : _supportedFormats) {
		if (e.format == format && e.colorSpace == colorSpace) {
			return true;
		}
	}
	return false;
}

bool AX_VkPhysicalDevice::findQueueFamilyIndex(AX_VkQueueFamilyIndex& outIndex, VkQueueFlags hasFlags) const {
	u32 i = 0;
	for (auto& p : _queueFamilyProps) {
		if (p.queueFlags & hasFlags) {
			outIndex = static_cast<AX_VkQueueFamilyIndex>(i);
			return true;
		}
		i++;
	}
	return false;
}

bool AX_VkPhysicalDevice::findGraphicFamilIndex(AX_VkQueueFamilyIndex& outIndex) const {
	return findQueueFamilyIndex(outIndex, VK_QUEUE_GRAPHICS_BIT);
}

bool AX_VkPhysicalDevice::findComputeQueueFamilyIndex(AX_VkQueueFamilyIndex& outIndex) const {
	u32 i = 0;
	AX_VkQueueFamilyIndex graphQueueCanCompute = AX_VkQueueFamilyIndex::Invalid;

	for (auto& p : _queueFamilyProps) {
		if (p.queueFlags & VK_QUEUE_COMPUTE_BIT) {

			if (p.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				graphQueueCanCompute = static_cast<AX_VkQueueFamilyIndex>(i);
			} else {
				outIndex = static_cast<AX_VkQueueFamilyIndex>(i);
				return true; // pick non-graph compute queue
			}
		}
		i++;
	}

	if (graphQueueCanCompute != AX_VkQueueFamilyIndex::Invalid) {
		outIndex = graphQueueCanCompute;
		return true;
	}

	return false;
}

bool AX_VkPhysicalDevice::findMemoryTypeIndex(AX_VkQueueFamilyIndex& outIndex, VkFlags typeBits, VkMemoryPropertyFlags requireMask) const {
AX_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")

	// Search memory types to find first index with those properties
	for (u32 i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
		if ((typeBits & 1) == 1) {
			// Type is available, does it match user properties?
			if ((_memProps.memoryTypes[i].propertyFlags & requireMask) == requireMask) {
				outIndex = static_cast<AX_VkQueueFamilyIndex>(i);
				return true;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;

AX_GCC_WARNING_POP()
}

AX_VkPhysicalDevice::Features::Features() {

	v10.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	v11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	v12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	v13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

	v10.pNext = &v11;
	v11.pNext = &v12;
	v12.pNext = &v13;
}

void AX_VkPhysicalDevice::create(Int index, VkInstance inst, VkPhysicalDevice phyDev) {
	_index = index;
	_inst = inst;
	_phyDev = phyDev;

	vkGetPhysicalDeviceProperties(_phyDev, &_props);

	vkGetPhysicalDeviceFeatures2(phyDev, &_features.v10);

	{
		_deviceMemorySize = 0;
		vkGetPhysicalDeviceMemoryProperties(_phyDev, &_memProps);
		for (uint32_t i = 0; i < _memProps.memoryHeapCount; i++) {
			auto& m = _memProps.memoryHeaps[i];
			if (m.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
				_deviceMemorySize += m.size;
			}
		}
	}

	{
		uint32_t count = 0;
		VkResult err   = vkEnumerateDeviceExtensionProperties(_phyDev, nullptr, &count, nullptr);
		AX_VkUtil::throwIfError(err);

		_extensionProps.resize(count);
		err = vkEnumerateDeviceExtensionProperties(_phyDev, nullptr, &count, _extensionProps.data());
		AX_VkUtil::throwIfError(err);
	}

	_majorVersion = AX_VkUtil::makeApiVersionMajor(_props.apiVersion);
	_minorVersion = AX_VkUtil::makeApiVersionMinor(_props.apiVersion);

	{	// get queueFamilyIndex
		uint32_t count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(_phyDev, &count, nullptr);
		assert(count >= 1);

		_queueFamilyProps.resize(count);
		vkGetPhysicalDeviceQueueFamilyProperties(_phyDev, &count, _queueFamilyProps.data());
		assert(count >= 1);
	}

	logInfo(false);
}

void AX_VkPhysicalDevice::logInfo(bool extensions) const {
	TempString s;

	FmtTo(s,"Vulkan Device[{}]:\n"
			"  deviceName = {}\n"
			"  deviceID = {}\n"
			"  vendorID = {}\n"
			"  driverVersion = {}\n"
			"  majorVersion = {}\n"
			"  minorVersion = {}\n",
			_index,
			deviceName(),
			_props.deviceID,
			_props.vendorID,
			_props.driverVersion,
			_majorVersion,
			_minorVersion);

	if (extensions) {
		s.append("  Extensions:\n");
		for (auto& e : _extensionProps) {
			FmtTo(s, "    {}\n", StrView_c_str(e.extensionName));
		}
	}

	AX_LOG("{}", s);
}

bool AX_VkPhysicalDevice::hasExtension(const char* name) {
	for (auto& e : _extensionProps) {
		if (ZStrUtil::equals(name, e.extensionName))
			return true;
	}
	return false;
}

AX_VkDevice& AX_VkDevice::create(AX_VkPhysicalDevice& phyDev) {
	destroy();
	_phyDev = &phyDev;

#if 0
	_enabledFeatures = {};
	_enabledFeatures.largePoints				= phyDev.features().largePoints;
	_enabledFeatures.wideLines					= phyDev.features().wideLines;
	_enabledFeatures.fillModeNonSolid			= phyDev.features().fillModeNonSolid;
	//_enabledFeatures.fullDrawIndexUint32		= phyDev.features().fullDrawIndexUint32;
	//_enabledFeatures.robustBufferAccess		= phyDev.features().robustBufferAccess;
	_enabledFeatures.textureCompressionETC2		= phyDev.features().textureCompressionETC2;
	_enabledFeatures.textureCompressionASTC_LDR = phyDev.features().textureCompressionASTC_LDR;
	_enabledFeatures.textureCompressionBC		= phyDev.features().textureCompressionBC;
	//_enabledFeatures.multiViewport			= phyDev.features().multiViewport;
	//_enabledFeatures.alphaToOne				= 
	//_enabledFeatures.depthBounds				= 
	//_enabledFeatures.depthClamp				= 
	//_enabledFeatures.depthBiasClamp			= 
	_enabledFeatures.geometryShader				= phyDev.features().geometryShader;
	_enabledFeatures.tessellationShader			= phyDev.features().tessellationShader;
	_enabledFeatures.shaderFloat64				= phyDev.features().shaderFloat64;
#else
	_enabledFeatures = phyDev.features(); // enable all the hardware support
#endif

	Array<const char*, 16> enabledExtensionNames;
	if (!_addEnabledExtensions(enabledExtensionNames, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
		throw Error_Undefined();

// optional extensions
	_addEnabledExtensions(enabledExtensionNames, VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
	// already included in Vulkan 1.1
	_addEnabledExtensions(enabledExtensionNames, VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
	_addEnabledExtensions(enabledExtensionNames, VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);

//------

	if (!phyDev.findGraphicFamilIndex(_graphQueueFamilyIndex)) {
		throw Error_Undefined();
	}

	float queuePriorities[1] = {0.0};
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType						= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext						= nullptr;
	queueCreateInfo.queueCount					= 1;
	queueCreateInfo.pQueuePriorities			= queuePriorities;
	queueCreateInfo.queueFamilyIndex			= ax_enum_int(_graphQueueFamilyIndex);

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext						= &_enabledFeatures.v10;
	deviceCreateInfo.flags						= 0;
	deviceCreateInfo.queueCreateInfoCount		= 1;
	deviceCreateInfo.pQueueCreateInfos			= &queueCreateInfo;
	deviceCreateInfo.enabledLayerCount			= 0;
	deviceCreateInfo.ppEnabledLayerNames		= nullptr;
	deviceCreateInfo.enabledExtensionCount		= AX_VkUtil::castUInt32(enabledExtensionNames.size());
	deviceCreateInfo.ppEnabledExtensionNames	= enabledExtensionNames.data();
	deviceCreateInfo.pEnabledFeatures			= nullptr; // must be null when pNext = VkPhysicalDeviceFeatures2

	auto err = vkCreateDevice(*_phyDev, &deviceCreateInfo, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);

	return *this;
}

void AX_VkDevice::destroy() {
	if (_handle) {
		vkDeviceWaitIdle(_handle);
		vkDestroyDevice(_handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
	}
}

bool AX_VkDevice::_addEnabledExtensions(IArray<const char*>& arr, const char* name) {
	if (!_phyDev->hasExtension(name))
		return false;
	arr.append(name);
	return true;
}

void AX_VkDevice::getQueue(AX_VkDeviceQueue &outQueue, AX_VkQueueFamilyIndex queueFamilyIndex, u32 queueIndex) {
	outQueue._dev = this;
	vkGetDeviceQueue(_handle, ax_enum_int(queueFamilyIndex), queueIndex, &outQueue._handle);
}

void AX_VkDevice::_setObjectDebugName(VkObjectType objectType, void* objectHandle, StrView name) {
	TempString tmpName = name;

	VkDebugUtilsObjectNameInfoEXT info = {};
	info.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	info.objectType		= objectType;
	info.objectHandle	= reinterpret_cast<u64>(objectHandle);
	info.pObjectName	= tmpName.c_str();

	auto* ext = AX_VkExtProcList::s_instance();
	if (ext->vkSetDebugUtilsObjectNameEXT) {
		auto err = ext->vkSetDebugUtilsObjectNameEXT(_handle, &info);
		AX_VkUtil::throwIfError(err);
	}
}

void AX_VkDevice::_setObjectDebugTag(VkObjectType objectType, void* objectHandle, u64 tagName, ByteSpan tag) {
	VkDebugUtilsObjectTagInfoEXT info = {};
	info.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT;
	info.objectType		= objectType;
	info.objectHandle	= reinterpret_cast<u64>(objectHandle);
	info.tagName		= tagName;
	info.tagSize		= SafeCast(tag.size());
	info.pTag			= tag.data();

	auto* ext = AX_VkExtProcList::s_instance();
	if (ext->vkSetDebugUtilsObjectTagEXT) {
		auto err = ext->vkSetDebugUtilsObjectTagEXT(_handle, &info);
		AX_VkUtil::throwIfError(err);
	}
}

void AX_VkInstance::create(AX_VkInstanceCreateInfo& info) {
	destroy();

	VkResult err = vkEnumerateInstanceVersion(&info.appInfo.apiVersion);
	AX_VkUtil::throwIfError(err);

	info.enabledLayerCount			= AX_VkUtil::castUInt32(info._enabledLayerNames.size());
	info.ppEnabledLayerNames		= info._enabledLayerNames.data();

	info.enabledExtensionCount		= AX_VkUtil::castUInt32(info._enabledExtensionNames.size());
	info.ppEnabledExtensionNames	= info._enabledExtensionNames.data();

	err = vkCreateInstance(&info, AX_VkUtil::allocCallbacks(), &_instance);
	AX_VkUtil::throwIfError(err);
}

void AX_VkInstance::destroy() {
	if (_instance) {
		vkDestroyInstance(_instance, AX_VkUtil::allocCallbacks());
		_instance = VK_NULL_HANDLE;
	}
}

void AX_VkRenderPass::destroy() {
	if (_handle) {
		vkDestroyRenderPass(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = VK_NULL_HANDLE;
	}

}

AX_VkRenderPass& AX_VkRenderPass::create(AX_VkDevice& dev, VkRenderPassCreateInfo& renderPassCreateInfo) {
	destroy();
	_dev = &dev;

	auto err = vkCreateRenderPass(dev, &renderPassCreateInfo, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);

	return *this;
}

AX_VkRenderPass& AX_VkRenderPass::create(AX_VkDevice& dev, VkFormat colorFormat, VkFormat depthStencilFormat, bool clear) {
	bool hasDepth = depthStencilFormat != VK_FORMAT_UNDEFINED;

	Array<VkAttachmentDescription, 2> attachmentDescription;

	VkAttachmentReference colorReference = {};
	VkAttachmentReference depthReference = {};

	colorReference.attachment = AX_VkUtil::castUInt32(attachmentDescription.size());
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	{
		auto& dst = attachmentDescription.emplaceBack();
		dst.format			= colorFormat;
		dst.flags			= 0;
		dst.samples			= VK_SAMPLE_COUNT_1_BIT;
		dst.loadOp			= clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		dst.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		dst.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		dst.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		dst.initialLayout	= clear ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ;
		dst.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}

	if (hasDepth) {
		depthReference.attachment = AX_VkUtil::castUInt32(attachmentDescription.size());
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		{
			auto& dst			= attachmentDescription.emplaceBack();
			dst.format			= depthStencilFormat;
			dst.flags			= 0;
			dst.samples			= VK_SAMPLE_COUNT_1_BIT;
			dst.loadOp			= clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			dst.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			dst.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.initialLayout	= clear ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ;
			dst.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
	}
		
	VkSubpassDescription subpassDesc = {};
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.flags = 0;
	subpassDesc.inputAttachmentCount		= 0;
	subpassDesc.pInputAttachments			= nullptr;
	subpassDesc.colorAttachmentCount		= 1;
	subpassDesc.pColorAttachments			= &colorReference;
	subpassDesc.pResolveAttachments			= nullptr;
	subpassDesc.pDepthStencilAttachment		= hasDepth ? &depthReference : nullptr;
	subpassDesc.preserveAttachmentCount		= 0;
	subpassDesc.pPreserveAttachments		= nullptr;

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext				= nullptr;
	renderPassCreateInfo.flags				= 0;
	renderPassCreateInfo.attachmentCount	= AX_VkUtil::castUInt32(attachmentDescription.size());
	renderPassCreateInfo.pAttachments		= attachmentDescription.data();
	renderPassCreateInfo.subpassCount		= 1;
	renderPassCreateInfo.pSubpasses			= &subpassDesc;
	renderPassCreateInfo.dependencyCount	= 0;
	renderPassCreateInfo.pDependencies		= nullptr;

	return create(dev, renderPassCreateInfo);
}

void AX_VkPhysicalDeviceList::create(VkInstance inst) {
	uint32_t count = 0;
	VkResult err = vkEnumeratePhysicalDevices(inst, &count, nullptr);
	AX_VkUtil::throwIfError(err);

	if (count <= 0) {
		AX_LOG("No vulkan physical device");
		throw Error_Undefined();
	}

	Array<VkPhysicalDevice, 8> tmp;

	tmp.resize(count);
	err = vkEnumeratePhysicalDevices(inst, &count, tmp.data());
	AX_VkUtil::throwIfError(err);

	_devices.resize(count);

	Int i = 0;
	for (auto& dev : tmp) {
		_devices[i].create(i, inst, dev);
		i++;
	}
}

void AX_VkSemaphore::destroy() {
	if (_handle) {
		vkDestroySemaphore(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

AX_VkSemaphore& AX_VkSemaphore::create(AX_VkDevice& dev) {
	destroy();
	_dev = &dev;
	VkSemaphoreCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	auto err = vkCreateSemaphore(*_dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);
	return *this;
}

void AX_VkTimelineSemaphore::destroy() {
	if (_handle) {
		vkDestroySemaphore(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void AX_VkTimelineSemaphore::signal(u64 value) {
	VkSemaphoreSignalInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
	info.semaphore = _handle;
	info.value = value;
	auto err = vkSignalSemaphore(*_dev, &info);
	AX_VkUtil::throwIfError(err);
}

AX_VkTimelineSemaphore& AX_VkTimelineSemaphore::create(AX_VkDevice& dev, u64 initialValue) {
	destroy();
	_dev = &dev;
	VkSemaphoreCreateInfo info = {};
	VkSemaphoreTypeCreateInfo typeInfo = {};
	typeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
	typeInfo.pNext = nullptr;
	typeInfo.initialValue = initialValue;
	
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = &typeInfo;
	info.flags = 0;
	auto err = vkCreateSemaphore(*_dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);
	return *this;
}

bool AX_VkTimelineSemaphore::wait(u64 value, const Nanoseconds& timeout) {
	VkSemaphoreWaitInfo info = {};
	info.sType				 = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	info.semaphoreCount		 = 1;
	info.pSemaphores		 = &_handle;
	info.pValues			 = &value;
	auto err				 = vkWaitSemaphores(*_dev, &info, SafeCast(timeout.value));
	if (err == VK_TIMEOUT) return false;
	if (err == VK_SUCCESS) return true;

	AX_VkUtil::throwIfError(err);
	return false;
}

void AX_VkCommandPool::destroy() {
	if (_handle) {
		vkDestroyCommandPool(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

AX_VkCommandPool& AX_VkCommandPool::create(AX_VkDevice& dev, AX_VkQueueFamilyIndex queueFamilyIndex) {
	destroy();
	_dev = &dev;
	VkCommandPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	info.queueFamilyIndex = ax_enum_int(queueFamilyIndex);

	auto err = vkCreateCommandPool(dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);

	return *this;
}

AX_VkCommandBuffer& AX_VkCommandBuffer::create(AX_VkCommandPool& cmdPool) {
	destroy();
	_cmdPool = &cmdPool;

	VkCommandBufferAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.commandPool = cmdPool;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = 1;

	auto err = vkAllocateCommandBuffers(*cmdPool.device(), &info, &_handle);
	AX_VkUtil::throwIfError(err);

	return *this;
}

void AX_VkCommandBuffer::destroy() {
	if (_handle) {
		vkFreeCommandBuffers(*_cmdPool->device(), *_cmdPool, 1, &_handle);
		_handle = nullptr;
		_cmdPool = nullptr;
	}
}

void AX_VkCommandBuffer::resetAndReleaseResource() {
	vkResetCommandBuffer(_handle, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
}

void AX_VkCommandBuffer::beginCommand() {
	VkCommandBufferBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = nullptr;
	info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	info.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(_handle, &info);
}

void AX_VkCommandBuffer::endCommand() {
	vkEndCommandBuffer(_handle);
}

void AX_VkCommandBuffer::pipelineBarrier(
	AX_VkQueueFamilyIndex srcQueueFamilyIndex,
	AX_VkQueueFamilyIndex dstQueueFamilyIndex,
	VkImage image
) {
	VkImageMemoryBarrier barrier = {};
	barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext							= nullptr;
	barrier.srcAccessMask					= 0;
	barrier.dstAccessMask					= 0;
	barrier.oldLayout						= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	barrier.newLayout						= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	barrier.srcQueueFamilyIndex				= ax_enum_int(srcQueueFamilyIndex);
	barrier.dstQueueFamilyIndex				= ax_enum_int(dstQueueFamilyIndex);
	barrier.image							= image;
	barrier.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel	= 0;
	barrier.subresourceRange.levelCount		= 1;
	barrier.subresourceRange.baseArrayLayer	= 0;
	barrier.subresourceRange.layerCount		= 1;

	vkCmdPipelineBarrier(	_handle,
							VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
							VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
							0, 
							0, nullptr, // memory barrier
							0, nullptr, // buffer memory barrier
							1, &barrier);
}

#if AX_DEBUG_NAME
void AX_VkCommandBuffer::setDebugName(const String& name) {
	if (!_cmdPool) {
		AX_ASSERT(false);
		return;
	}
	auto* dev = _cmdPool->device();
	if (!dev) {
		AX_ASSERT(false);
		return;
	}
	dev->setObjectDebugName(_handle, name);
}
#endif

void AX_VkCommandBuffer::debugLabelBegin(const String& name, const Color4f& color) {
	auto* ext = AX_VkExtProcList::s_instance();
	if (ext->vkCmdBeginDebugUtilsLabelEXT) {

		VkDebugUtilsLabelEXT info = {};
		info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		info.pLabelName = name.c_str();
		AX_VkUtil::setFloat4(info.color, color);
		ext->vkCmdBeginDebugUtilsLabelEXT(_handle, &info);
	}
}

void AX_VkCommandBuffer::debugLabelEnd() {
	auto* ext = AX_VkExtProcList::s_instance();
	if (ext->vkCmdEndDebugUtilsLabelEXT) {
		ext->vkCmdEndDebugUtilsLabelEXT(_handle);
	}
}

void AX_VkImage::destroy() {
	if (_handle) {
		if (!_isBackBuffer) {
			vkDestroyImage(*_dev, _handle, AX_VkUtil::allocCallbacks());
		}
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
		_isBackBuffer = false;
	}
}

AX_VkImage& AX_VkImage::createImage2D(
	AX_VkDevice& dev,
	VkExtent2D frameSize,
	VkFormat format,
	uint32_t mipLevels,
	VkImageUsageFlags usage,
	VkImageLayout initialLayout
) {
	VkImageCreateInfo info = {};
	info.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.pNext			= nullptr;
	info.imageType		= VK_IMAGE_TYPE_2D;
	info.format			= format;
	info.extent.width	= frameSize.width;
	info.extent.height	= frameSize.height;
	info.extent.depth	= 1;
	info.mipLevels		= mipLevels;
	info.arrayLayers	= 1;
	info.samples		= VK_SAMPLE_COUNT_1_BIT;
	info.tiling			= VK_IMAGE_TILING_OPTIMAL;
	info.usage			= usage;
	info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
	info.flags			= 0;
	info.initialLayout	= initialLayout;
	_create(dev, info);

	return *this;
}

AX_VkImage& AX_VkImage::createDepthStencil(
	AX_VkDevice& dev,
	VkExtent2D frameSize,
	VkFormat format
) {
	return createImage2D(dev, frameSize, format, 1, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_UNDEFINED);
}

AX_VkImage& AX_VkImage::createFromBackBuffer(AX_VkDevice& dev, VkImage handle, VkExtent2D frameSize, VkFormat format) {
	destroy();
	_dev		  = &dev;
	_handle		  = handle;
	_isBackBuffer = true;
	_size.width	  = frameSize.width;
	_size.height  = frameSize.height;
	_size.depth	  = 1;
	_format		  = format;

	return *this;
}

void AX_VkImage::setLayout(AX_VkCommandBuffer& cmdBuffer, VkImageLayout newLayout, VkPipelineStageFlags newStage) {
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.oldLayout = _layout;
	barrier.newLayout = newLayout;
	barrier.image = _handle;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel	= 0;
	barrier.subresourceRange.levelCount		= 1;
	barrier.subresourceRange.baseArrayLayer	= 0;
	barrier.subresourceRange.layerCount		= 1;

	switch (newLayout) {
		/* Make sure anything that was copying from this image has completed */
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;										break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;								break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:	barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;						break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;	break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;										break;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:					barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;											break;
		default:												barrier.dstAccessMask = 0;																	break;
	}
	
	vkCmdPipelineBarrier(	cmdBuffer,
							_stage,
							newStage,
							0,
							0, nullptr, // memory barrier
							0, nullptr, // buffer barrier
							1, &barrier);
	_layout = newLayout;
	_stage = newStage;
}

VkMemoryRequirements AX_VkImage::getMemoryRequirements() {
	VkMemoryRequirements o = {};
	vkGetImageMemoryRequirements(*_dev, _handle, &o);
	return o;
}

void AX_VkImage::_create(AX_VkDevice& dev, VkImageCreateInfo& info) {
	destroy();
	_dev	= &dev;
	_format = info.format;
	_size	= info.extent;
	_layout = info.initialLayout;
	_stage	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	_isBackBuffer = false;

	auto err = vkCreateImage(dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);
}

void AX_VkImageView::destroy() {
	if (_handle) {
		vkDestroyImageView(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = VK_NULL_HANDLE;
	}
}

AX_VkImageView& AX_VkImageView::create(AX_VkImage& image) {
	return create(*image.device(), image, image.format());
}

AX_VkImageView& AX_VkImageView::create(AX_VkDevice& dev, VkImage image, VkFormat format) {
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.image = image;
	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.format = format;
	info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	{
		auto& aspectMask = info.subresourceRange.aspectMask;
		if (AX_VkUtil::formatHasDepth(format))   aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
		if (AX_VkUtil::formatHasStencil(format)) aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		if (!aspectMask) aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	info.subresourceRange.baseMipLevel		= 0;
	info.subresourceRange.levelCount		= 1;
	info.subresourceRange.baseArrayLayer	= 0;
	info.subresourceRange.layerCount		= 1;

	return create(dev, image, info);
}

AX_VkImageView& AX_VkImageView::create(AX_VkDevice& dev, VkImage image, VkImageViewCreateInfo& info) {
	destroy();
	_dev = &dev;
	_format = info.format;

	auto err = vkCreateImageView(dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);

	return *this;
}

void AX_VkSwapchainKHR::destroy() {
	if (_handle) {
		vkDestroySwapchainKHR(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void AX_VkSwapchainKHR::create(
	AX_VkDevice& dev,
	AX_VkSurfaceKHR& surface,
	const VkSurfaceFormatKHR& format,
	Int imageCount_,
	VkPresentModeKHR presentMode,
	AX_VkSwapchainKHR* oldSwapchain
) {
	destroy();
	_dev = &dev;

	if (!surface) throw Error_Undefined();
	if (!surface.isSupportFormat(format)) throw Error_Undefined();

	auto imageCount = AX_VkUtil::castUInt32(imageCount_);

	auto cap = surface.getCapabilities();
	if (imageCount < cap.minImageCount) throw Error_Undefined();
	if (imageCount > cap.maxImageCount) throw Error_Undefined();

	Array<u32, 2> queueFamilyIndices;
	queueFamilyIndices.emplaceBack(ax_enum_int(surface.graphQueueFamilyIndex()));
	if (surface.isPresentQueueIsSeparated()) {
		queueFamilyIndices.emplaceBack(ax_enum_int(surface.presentQueueFamilyIndex()));
	}

	VkExtent2D imageExtent = cap.currentExtent;

	VkSwapchainCreateInfoKHR info = {};
	info.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.pNext					= nullptr;
	info.flags					= 0;
	info.surface				= surface;
	info.minImageCount			= imageCount;
	info.imageFormat			= format.format;
	info.imageColorSpace		= format.colorSpace;
	info.imageExtent			= imageExtent;
	info.imageArrayLayers		= 1;
	info.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	info.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount	= AX_VkUtil::castUInt32(queueFamilyIndices.size());
	info.pQueueFamilyIndices	= queueFamilyIndices.data();
	info.preTransform			= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	info.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.presentMode			= presentMode;
	info.clipped				= VK_TRUE;
	info.oldSwapchain			= oldSwapchain ? oldSwapchain->handle() : nullptr;

	auto err = vkCreateSwapchainKHR(*_dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);
}

void AX_VkSwapchainKHR::getImages(IArray<VkImage>& outImages) {
	outImages.clear();

	uint32_t count = 0;
	VkResult err   = vkGetSwapchainImagesKHR(*_dev, _handle, &count, nullptr);
	AX_VkUtil::throwIfError(err);

	outImages.resize(count);
	err = vkGetSwapchainImagesKHR(*_dev, _handle, &count, outImages.data());
	AX_VkUtil::throwIfError(err);
}

VkResult AX_VkSwapchainKHR::acquireNextImage(u32& outImageIndex, VkSemaphore semaphoreToSignal, VkFence fenceToSignal, Nanoseconds timeout) {
	if (timeout.value < 0) timeout.value = 0;
	u64 timeout_u64 = static_cast<u64>(timeout.value);

	return vkAcquireNextImageKHR(*_dev, _handle, timeout_u64, semaphoreToSignal, fenceToSignal, &outImageIndex);
}

void AX_VkFramebuffer::destroy() {
	if (_handle) {
		vkDestroyFramebuffer(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

AX_VkFramebuffer& AX_VkFramebuffer::create(
	AX_VkDevice& dev,
	VkRenderPass renderPass,
	Span<VkImageView> attachments,
	VkExtent2D frameSize
) {
	destroy();
	_dev = &dev;
	_frameSize = frameSize;

	VkFramebufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.pNext = nullptr;
	info.renderPass = renderPass;
	info.attachmentCount = AX_VkUtil::castUInt32(attachments.size());
	info.pAttachments = attachments.data();
	info.width  = frameSize.width;
	info.height = frameSize.height;
	info.layers = 1;

	auto err = vkCreateFramebuffer(dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);

	return *this;
}

void AX_VkDeviceMemory::destroy() {
	if (_handle) {
		vkFreeMemory(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

AX_VkDeviceMemory& AX_VkDeviceMemory::createForImage(AX_VkImage& img, VkMemoryPropertyFlags requireMask) {
	destroy();

	auto req = img.getMemoryRequirements();
	auto* dev = img.device();
	_create(*dev, SafeCast(req.size), req.memoryTypeBits, requireMask);

	auto err = vkBindImageMemory(*dev, img, _handle, 0);
	AX_VkUtil::throwIfError(err);

	return *this;
}

AX_VkDeviceMemory& AX_VkDeviceMemory::createForBuffer(AX_VkBuffer& buf, VkMemoryPropertyFlags requireMask) {
	destroy();

	auto req = buf.getMemoryRequirements();
	auto* dev = buf.device();
	_create(*dev, SafeCast(req.size), req.memoryTypeBits, requireMask);

	auto err = vkBindBufferMemory(*dev, buf, _handle, 0);
	AX_VkUtil::throwIfError(err);

	return *this;
}

void AX_VkDeviceMemory::copyData(
	ByteSpan data,
	Int offset,
	VkMemoryMapFlags flags
) {
	if (data.size() <= 0) return;
	auto span = mapMemory(Range_BeginSize(offset, data.size()), flags);
	try {
		span.copyValues(data);
	} catch (...) {
		unmapMemory();
		throw;
	}
	unmapMemory();
}

MutByteSpan AX_VkDeviceMemory::mapMemory(IntRange range, VkMemoryMapFlags flags) {
	void* outPtr = nullptr;
	auto offset = AX_VkUtil::castVkDeviceSize(range.begin());
	auto size =   AX_VkUtil::castVkDeviceSize(range.size());

	auto err = vkMapMemory(*_dev, _handle, offset, size, flags, &outPtr);
	AX_VkUtil::throwIfError(err);
	return MutByteSpan(static_cast<Byte*>(outPtr), SafeCast(size));
}

void AX_VkDeviceMemory::unmapMemory() {
	vkUnmapMemory(*_dev, _handle);
}

void AX_VkDeviceMemory::_create(
	AX_VkDevice& dev, 
	Int bufferSize, 
	VkFlags memoryTypeBits,
	VkMemoryPropertyFlags requireMask
) {
	auto memTypeIndex = AX_VkQueueFamilyIndex::Invalid;
	if (!dev.physicalDevice()->findMemoryTypeIndex(memTypeIndex, memoryTypeBits, requireMask))
		throw Error_Undefined();

	_create(dev, bufferSize, memTypeIndex);
}

void AX_VkDeviceMemory::_create(AX_VkDevice& dev, Int bufferSize, AX_VkQueueFamilyIndex memTypeIndex) {
	destroy();
	_dev = &dev;
	_bufferSize = bufferSize;

	VkMemoryAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.allocationSize = AX_VkUtil::castVkDeviceSize(bufferSize);
	info.memoryTypeIndex = ax_enum_int(memTypeIndex);
	auto err = vkAllocateMemory(dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);
}

void AX_VkDeviceMemory::_fillVkMappedMemoryRange(IArray<VkMappedMemoryRange>& outArray, Span<IntRange> ranges) {
	outArray.clear();
	outArray.ensureCapacity(ranges.size());

	if (_bufferSize <= 0) return;

	auto bufferSize = AX_VkUtil::castVkDeviceSize(_bufferSize);
	auto atomSize   = _dev->physicalDevice()->props().limits.nonCoherentAtomSize;

	for (auto& src : ranges) {
		if (src.size() <= 0) continue;

		if (!IntRange(_bufferSize).contains(src))
			throw Error_Undefined();

		auto& dst  = outArray.emplaceBack();
		dst.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		dst.pNext  = nullptr;
		dst.memory = _handle;
		dst.offset = AX_VkUtil::castVkDeviceSize(src.begin());
		dst.size   = AX_VkUtil::castVkDeviceSize(src.size());
		dst.size   = Math::alignTo(dst.size, atomSize);
		if (dst.size > bufferSize) 
			dst.size = VK_WHOLE_SIZE;
	}
}

void AX_VkDeviceMemory::flushMappedMemoryRanges(Span<IntRange> ranges) {
	if (!_dev) { AX_ASSERT(false); return; }

	Array<VkMappedMemoryRange, 32> arr;
	_fillVkMappedMemoryRange(arr, ranges);
	if (arr.size() <= 0) return;
	vkFlushMappedMemoryRanges(*_dev, AX_VkUtil::castUInt32(arr.size()), arr.data());
}

void AX_VkDeviceMemory::InvalidateMappedMemoryRanges(Span<IntRange> ranges) {
	if (!_dev) { AX_ASSERT(false); return; }

	Array<VkMappedMemoryRange, 32> arr;
	_fillVkMappedMemoryRange(arr, ranges);
	if (arr.size() <= 0) return;
	vkInvalidateMappedMemoryRanges(*_dev, AX_VkUtil::castUInt32(arr.size()), arr.data());
}

void AX_VkBuffer::destroy() {
	if (_handle) {
		vkDestroyBuffer(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void AX_VkBuffer::create(AX_VkDevice& dev, VkDeviceSize bufferSize, VkBufferUsageFlags usage) {
	if (_handle && _dev == &dev && bufferSize == _bufferSize && usage == _usage) 
		return;

	destroy();
	_dev = &dev;

	VkBufferCreateInfo info = {};
	info.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.pNext					= nullptr;
	info.flags					= 0;
	info.size					= Math::max(bufferSize, u64(8)); // ensure not too small
	info.usage					= usage;
	info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount	= 0;
	info.pQueueFamilyIndices	= nullptr;

	auto err = vkCreateBuffer(*_dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);

	_bufferSize = bufferSize;
}

VkMemoryRequirements AX_VkBuffer::getMemoryRequirements() {
	VkMemoryRequirements o;
	vkGetBufferMemoryRequirements(*_dev, _handle, &o);
	return o;
}

void AX_VkFence::destroy() {
	if (_handle != VK_NULL_HANDLE) {
		wait(UINT64_MAX);
		vkDestroyFence(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

AX_VkFence& AX_VkFence::create(AX_VkDevice& dev, bool signaled) {
	destroy();
	_dev = &dev;

	VkFenceCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	auto err = vkCreateFence(dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);

	return *this;
}

void AX_VkFence::reset() {
	auto err = vkResetFences(*_dev, 1, &_handle);
	AX_VkUtil::throwIfError(err);
}

bool AX_VkFence::check(bool doReset) {
	auto res = vkGetFenceStatus(*_dev, _handle);
	if (res == VK_NOT_READY) return false;

	if (doReset) reset();

	if (res == VK_SUCCESS) return true;
	AX_VkUtil::throwIfError(res);
	return false;
}

bool AX_VkFence::wait(uint64_t nanoseconds) {
	auto err = vkWaitForFences(*_dev, 1, &_handle, true, nanoseconds);
	if (err == VK_TIMEOUT)
		return false;

	AX_VkUtil::throwIfError(err);

	return true;
}

void AX_VkShaderModule::destroy() {
	if (_handle) {
		vkDestroyShaderModule(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void AX_VkShaderModule::create(AX_VkDevice& dev, ByteSpan byteCode) {
	destroy();
	_dev = &dev;

	if (byteCode.size() % 4 != 0) {
		throw Error_Undefined();
	}

	VkShaderModuleCreateInfo info = {};
	info.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.pNext		= nullptr;
	info.flags		= 0;
	info.codeSize	= SafeCast(byteCode.size()); 
	info.pCode		= reinterpret_cast<const uint32_t*>(byteCode.data());

	auto err = vkCreateShaderModule(dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);
}

void AX_VkShaderModule::createFromFile(AX_VkDevice& dev, StrView filename) {
	FileMemMap byteCode(filename);
	create(dev, byteCode);
}

void AX_VkPipeline::destroy() {
	if (_handle) {
		vkDestroyPipeline(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void AX_VkPipelineLayout::destroy() {
	if (_handle) {
		vkDestroyPipelineLayout(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void AX_VkPipelineLayout::create(AX_VkDevice& dev, Span<VkDescriptorSetLayout> descriptorSets) {
	destroy();
	_dev = &dev;

	VkPipelineLayoutCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	info.setLayoutCount = AX_VkUtil::castUInt32(descriptorSets.size());
	info.pSetLayouts = descriptorSets.data();

	auto err = vkCreatePipelineLayout(dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);
}

void AX_VkPipeline::create(AX_VkDevice& dev, VkPipelineCache cache, const VkGraphicsPipelineCreateInfo& info) {
	destroy();
	_dev = &dev;
	auto err = vkCreateGraphicsPipelines(dev, cache, 1, &info, AX_VkUtil::allocCallbacks(), &_handle); 
	AX_VkUtil::throwIfError(err);
}

void AX_VkPipeline::create(AX_VkDevice& dev, VkPipelineCache cache, const VkComputePipelineCreateInfo& info) {
	destroy();
	_dev = &dev;
	auto err = vkCreateComputePipelines(dev, cache, 1, &info, AX_VkUtil::allocCallbacks(), &_handle); 
	AX_VkUtil::throwIfError(err);
}

void AX_VkDescriptorSetLayout::destroy() {
	if (_handle) {
		vkDestroyDescriptorSetLayout(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = nullptr;
		_dev = nullptr;
	}
}

void AX_VkDescriptorSetLayout::create(
	AX_VkDevice& dev,
	Span<VkDescriptorSetLayoutBinding> bindings, 
	Span<VkDescriptorBindingFlags> bindingFlags,
	VkDescriptorSetLayoutCreateFlags flags) 
{
	destroy();
	_dev = &dev;

	VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo = {};
	bindingFlagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	bindingFlagsInfo.pNext = nullptr;
	bindingFlagsInfo.pBindingFlags = bindingFlags.data();
	bindingFlagsInfo.bindingCount  = AX_VkUtil::castUInt32(bindingFlags.size());

	if (bindingFlags.size() != 0) {
		if (bindings.size() != bindingFlags.size()) {
			throw Error_Undefined("AX_VkDescriptorSetLayout::create() invalid bindingFlags.size()");
		}
	}

	VkDescriptorSetLayoutCreateInfo info = {};
	info.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.pNext			= bindingFlags.size() ? &bindingFlagsInfo : nullptr;
	info.flags			= flags;
	info.bindingCount	= AX_VkUtil::castUInt32(bindings.size());
	info.pBindings		= bindings.data();

	auto err = vkCreateDescriptorSetLayout(*_dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);
}

void AX_VkPipelineCache::destroy() {
	if (_handle) {
		vkDestroyPipelineCache(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void AX_VkPipelineCache::create(AX_VkDevice& dev) {
	destroy();
	_dev = &dev;

	VkPipelineCacheCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.initialDataSize = 0;
	info.pInitialData = nullptr;

	auto err = vkCreatePipelineCache(dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);
}

void AX_VkDeviceQueue::submit(Span<VkSubmitInfo> infos, VkFence fenceToSignal) {
	auto err = vkQueueSubmit(_handle, AX_VkUtil::castUInt32(infos.size()), infos.data(), fenceToSignal);
	if (!AX_VkUtil::checkResult(err)) {
		uint32_t count = 0;
		//			renderer_->procList()->vkGetQueueCheckpointDataNV(_swapChainData->_graphQueue, &count, nullptr);
		Array<VkCheckpointDataNV, 128> checkPoints;
		checkPoints.resize(count);

		for (auto& e : checkPoints) {
			e.sType				= VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV;
			e.pNext				= nullptr;
			e.pCheckpointMarker = nullptr;
			e.stage				= VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
		}

		//			renderer_->procList()->vkGetQueueCheckpointDataNV(_swapChainData->_graphQueue, &count,
		//checkPoints.data());

		for (auto& e : checkPoints.revForEach()) {
			AX_LOG("checkPoint {}", StrView_c_str(static_cast<const char*>(e.pCheckpointMarker)));
		}
	}
	AX_VkUtil::throwIfError(err);
}

void AX_VkDeviceQueue::submit(
	const WaitSemaphores&		waitSemaphores,
	Span<VkCommandBuffer>		commandBuffers,
	Span<VkSemaphore>			signalSemaphores,
	VkFence						fenceToSignal /*= VK_NULL_HANDLE*/
) {
	auto waitSem   = waitSemaphores.semaphores();
	auto waitStage = waitSemaphores.dstStageFlags();

	AX_ASSERT(waitSem.size() == waitStage.size());

	VkSubmitInfo info = {};
	info.sType		  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.pNext		  = nullptr;

	info.waitSemaphoreCount		= AX_VkUtil::castUInt32(waitSem.size());
	info.pWaitSemaphores		= waitSem.data();

	info.pWaitDstStageMask		= waitStage.data();

	info.commandBufferCount		= AX_VkUtil::castUInt32(commandBuffers.size());
	info.pCommandBuffers		= commandBuffers.data();

	info.signalSemaphoreCount	= AX_VkUtil::castUInt32(signalSemaphores.size());
	info.pSignalSemaphores		= signalSemaphores.data();

	submit(info, fenceToSignal);
}

void AX_VkDeviceQueue::_present(
	Span<VkSemaphore>		waitSemaphores,
	Span<VkSwapchainKHR>	swapchains,
	Span<u32>				imageIndexInSwapchains
) {
	if (swapchains.size() != imageIndexInSwapchains.size())
		throw Error_Undefined();

	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	info.waitSemaphoreCount	= AX_VkUtil::castUInt32(waitSemaphores.size());
	info.pWaitSemaphores	= waitSemaphores.data();

	info.swapchainCount		= AX_VkUtil::castUInt32(swapchains.size());
	info.pSwapchains		= swapchains.data();
	info.pImageIndices		= imageIndexInSwapchains.data();

#if 1
	info.pResults			= nullptr;
#else
	// per-swapchain results
	AX_ASSERT(swapchains.size() <= 16);
	Array<VkResult, 16>	results;
	results.resize(swapchains.size());
	info.pResults = result.data();
#endif

	auto err = vkQueuePresentKHR(_handle, &info);

	if (err == VK_ERROR_OUT_OF_DATE_KHR) {
		// swapchain is out of date (e.g. the window was resized) and
		// must be recreated:
		// auto d = _mdata.scopedLock();
		// d->swapChainDirty = true;

	} else if (err == VK_SUBOPTIMAL_KHR) {
		// swapchain is not as optimal as it could be, but the platform's
		// presentation engine will still present the image correctly.
	} else {
		AX_VkUtil::throwIfError(err);
	}
}

void AX_VkDeviceQueue::present(VkSemaphore waitSemaphore, VkSwapchainKHR swapchain, Span<Int> imageIndexInSwapchain) {
	Array<u32, 16>	tmp;
	tmp.appendRange(imageIndexInSwapchain, [](auto& e) -> u32 { return SafeCast(e); });
	present(waitSemaphore, swapchain, tmp);
}

static AX_VkExtProcList* AX_VkExtProcList_instance = nullptr;

void AX_VkExtProcList::s_create(VkInstance vkInst_) {
	if (AX_VkExtProcList_instance) return;

	static char buf[sizeof(AX_VkExtProcList)];
	AX_VkExtProcList_instance = new(buf) AX_VkExtProcList(vkInst_);
}

void AX_VkExtProcList::s_destroy() {
	if (AX_VkExtProcList_instance) {
		ax_call_destructor(AX_VkExtProcList_instance);
		AX_VkExtProcList_instance = nullptr;
	}
}

AX_VkExtProcList::AX_VkExtProcList(VkInstance vkInst_) {
	_vkInst = vkInst_;

	#define AX_MACRO_OP(E) \
		_getExtProc(E, #E); \
		if (!E) { \
			AX_LOG("Error: cannot find vulkan proc {}", #E); \
			/* throw Error_Undefined(); */ \
		} \
	//------
		AX_VkExtProcList_ENUM_LIST(AX_MACRO_OP);
	#undef AX_MACRO_OP
}

AX_VkExtProcList* AX_VkExtProcList::s_instance() {
	return AX_VkExtProcList_instance;
}

template<class FUNC> inline
void AX_VkExtProcList::_getExtProc(FUNC& outFunc, StrLit name) {
AX_GCC_WARNING_PUSH_AND_DISABLE("-Wcast-function-type-strict")
	outFunc = reinterpret_cast<FUNC>(vkGetInstanceProcAddr(_vkInst, name.c_str()));
AX_GCC_WARNING_POP()
}

void AX_VkDebugReportCallbackEXT::create(VkInstance inst, PFN_vkDebugReportCallbackEXT callback) {
	destroy();

	_inst = inst;

	VkDebugReportCallbackCreateInfoEXT info = {};
	info.sType								= VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	info.pNext								= nullptr;
	info.flags								= 0
				 //| VK_DEBUG_REPORT_DEBUG_BIT_EXT
				 //| VK_DEBUG_REPORT_INFORMATION_BIT_EXT
				 | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT
				 | VK_DEBUG_REPORT_ERROR_BIT_EXT;
	info.pfnCallback = callback;
	info.pUserData	 = this;

	auto* ext = AX_VkExtProcList::s_instance();
	if (ext->vkCreateDebugReportCallbackEXT) {
		ext->vkCreateDebugReportCallbackEXT(inst, &info, AX_VkUtil::allocCallbacks(), &_handle);
	}
}

void AX_VkDebugReportCallbackEXT::destroy() {
	if (_handle) {
		auto* ext = AX_VkExtProcList::s_instance();
		if (ext->vkDestroyDebugReportCallbackEXT) {
			ext->vkDestroyDebugReportCallbackEXT(_inst, _handle, AX_VkUtil::allocCallbacks());
		}
		_handle = nullptr;
	}
}

void AX_VkDescriptorPool::destroy() {
	if (_handle) {
		vkDestroyDescriptorPool(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void AX_VkDescriptorPool::create(AX_VkDevice& dev, Span<VkDescriptorPoolSize> poolSizes, Int maxSets, VkDescriptorPoolCreateFlags flags) {
	destroy();
	_dev = &dev;

	VkDescriptorPoolCreateInfo info = {};

	info.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.maxSets	   = AX_VkUtil::castUInt32(maxSets);
	info.poolSizeCount = AX_VkUtil::castUInt32(poolSizes.size());
	info.pPoolSizes	   = poolSizes.data();
	info.flags		   = flags;

	auto err = vkCreateDescriptorPool(*_dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
	AX_VkUtil::throwIfError(err);
}

VkDescriptorSet AX_VkDescriptorPool::allocDescriptorSet(VkDescriptorSetLayout descSetLayout) {
	VkDescriptorSetAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.descriptorPool = _handle;
	info.descriptorSetCount = 1;
	info.pSetLayouts = &descSetLayout;

	VkDescriptorSet outSet = VK_NULL_HANDLE;
	auto err = vkAllocateDescriptorSets(*_dev, &info, &outSet);
	AX_VkUtil::throwIfError(err);

	return outSet;
}

void AX_VkSampler::destroy() {
	if (_handle) {
		vkDestroySampler(*_dev, _handle, AX_VkUtil::allocCallbacks());
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void AX_VkSampler::create(AX_VkDevice& dev, const VkSamplerCreateInfo& info) {
	destroy();
	_dev = &dev;

	vkCreateSampler(dev, &info, AX_VkUtil::allocCallbacks(), &_handle);
}

VkDescriptorSetLayoutBinding& AX_VkDescriptorSetLayoutBindings::addBinding(VkDescriptorType			type,
																		   BindPoint				bindPoint,
																		   Int						descriptorCount,
																		   ShaderStageFlags			stageFlags,
																		   VkDescriptorBindingFlags flags) {
	auto& dst = bindings.emplaceBack();
	dst.binding = ax_enum_int(bindPoint);
	dst.descriptorType = type;
	dst.descriptorCount = AX_VkUtil::castUInt32(descriptorCount);
	dst.stageFlags = AX_VkUtil::getVkShaderStageFlagBits(stageFlags);
	dst.pImmutableSamplers = nullptr;

	bindingFlags.emplaceBack(flags);
	return dst;
}

} // namespace
#endif // AX_RENDERER_VK