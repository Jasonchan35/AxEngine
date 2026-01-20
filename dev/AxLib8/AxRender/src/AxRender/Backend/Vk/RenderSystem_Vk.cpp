module;

module AxRender;

#if AX_RENDERER_VK
import :RenderSystem_Vk;
import :GpuBuffer_Vk;
import :Material_Vk;
import :RenderContext_Vk;
import :RenderObjectManager_Vk;
import :RenderPass_Vk;
import :RenderRequest_Vk;
import :Material_Vk;
import :MeshObject_Vk;

namespace ax /*::AxRender*/ {

AX_RenderSystem_FunctionBodies(Vk)

RenderSystem_Vk::RenderSystem_Vk(const CreateDesc& desc)
: Base(desc)
{
	_createVkInstance();

	AX_VkExtProcList::s_create(_vkInst);

	if (_enableDebugReport) {
		_debugReportCallbackExt.create(_vkInst, s_debugReport);
	}

	_physicalDeviceList.create(_vkInst);
	auto* phyDev = _physicalDeviceList.defaultDevice();

	_adapterInfo.name = phyDev->deviceName();
	_adapterInfo.memorySize = phyDev->deviceMemorySize();

	bool supportBindless = true;
	auto& features = phyDev->features();

	auto& limits = phyDev->props().limits;
	
	_adapterInfo.maxTexture1DSize                = Vec1i::s_all(ax_safe_cast_from(limits.maxImageDimension1D));
	_adapterInfo.maxTexture2DSize                = Vec2i::s_all(ax_safe_cast_from(limits.maxImageDimension2D));
	_adapterInfo.maxTexture3DSize                = Vec3i::s_all(ax_safe_cast_from(limits.maxImageDimension3D));
	_adapterInfo.minMemoryMapAlignment           = ax_safe_cast_from(limits.minMemoryMapAlignment);
	_adapterInfo.minUniformBufferOffsetAlignment = ax_safe_cast_from(limits.minUniformBufferOffsetAlignment);
	
	auto bindlessRequire = [&](VkBool32 b, StrView name) {
		if (!b) {
			supportBindless = false;
			AX_ASSERT_MSG(false, Fmt("Bindless require features {}", name));
		}
	};

	#define E(FEATURE) \
		bindlessRequire(FEATURE, #FEATURE)
	//----
	E(features.v12.shaderSampledImageArrayNonUniformIndexing);
	E(features.v12.shaderSampledImageArrayNonUniformIndexing);
	E(features.v12.descriptorBindingSampledImageUpdateAfterBind);
	E(features.v12.shaderUniformBufferArrayNonUniformIndexing);
	E(features.v12.descriptorBindingUniformBufferUpdateAfterBind);
	E(features.v12.shaderStorageBufferArrayNonUniformIndexing);
	E(features.v12.descriptorBindingStorageBufferUpdateAfterBind);

	#undef E

	_device.create(*phyDev);

#if AX_RENDER_DEBUG_NAME
	_device.setDebugName("Renderer-Device");
#endif
	
	_createSparseBuffers();
}

RenderSystem_Vk::~RenderSystem_Vk() {
	destroy();
}

void RenderSystem_Vk::onDestroy() {
	RenderSystem_Backend::onDestroy();
	_sparseVertexBuffer.destroy();
	_sparseIndexBuffer.destroy();
}

void RenderSystem_Vk::_createVkInstance() {

#ifdef AX_NATIVE_UI_MACOSX
//	EnvVar::setValue("VULKAN_SDK", "/Applications/VulkanSDK/vulkansdk-macos-1.2.141.2/macOS");
//	EnvVar::setValue("PATH","$VULKAN_SDK/bin:$PATH");

	StrLit icdFile   = "/usr/local/share/vulkan/icd.d/MoltenVK_icd.json";
	if (!File::exists(icdFile)) {
		AX_LOG("Vulkan SDK file not found = {}", icdFile);
		throw Error_Undefined();
	}

	StrLit layerPath = "/usr/local/share/vulkan/explicit_layer.d";
	if (!FileDir::exists(layerPath)) {
		AX_LOG("Vulkan SDK path not found = {}", layerPath);
		throw Error_Undefined();
	}

	EnvVar::setValue("VK_ICD_FILENAMES",		icdFile);
	EnvVar::setValue("VK_LAYER_PATH",			layerPath);
#endif

	AX_VkInstanceCreateInfo info;
	info.logInfo();

	// extensions
	if (!info.enableExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
		_enableDebugReport = false;

	if (!info.enableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
		_enableDebugUtils = false;

	if (!info.enableExtension(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
		_enableDebugMarker = false;
	
	// VMA allocator support extensions
	info.enableExtension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
	info.enableExtension(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
	info.enableExtension(VK_KHR_MAINTENANCE_4_EXTENSION_NAME);
	info.enableExtension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
	info.enableExtension(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
	info.enableExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	info.enableExtension(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME);
	info.enableExtension(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME);
	info.enableExtension(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);

//---- surface
	if (!info.enableExtension(VK_KHR_SURFACE_EXTENSION_NAME				)) throw Error_Undefined();

	#if AX_NATIVE_UI_WIN32
		if (!info.enableExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME	)) throw Error_Undefined();
	#elif AX_NATIVE_UI_X11
		if (!info.enableExtension(VK_KHR_XLIB_SURFACE_EXTENSION_NAME	)) throw Error_Undefined();
	#elif AX_NATIVE_UI_MACOSX
		if (!info.enableExtension(VK_MVK_MACOS_SURFACE_EXTENSION_NAME	)) throw Error_Undefined();
	//	if (!info.enableExtension(VK_EXT_METAL_SURFACE_EXTENSION_NAME	)) throw Error_Undefined();
	#endif

	// layers
	#if !AX_NATIVE_UI_MACOSX
		if (!info.enableLayer("VK_LAYER_KHRONOS_validation")) throw Error_Undefined();
	#endif

	_vkInst.create(info);
}

void RenderSystem_Vk::_createSparseBuffers() {
	_sparseVertexBuffer.create(_device, 32 * Math::GigaBytes, 
	                           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	                           VMA_MEMORY_USAGE_GPU_ONLY);

	_sparseIndexBuffer.create(_device, 8 * Math::GigaBytes,
	                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	                          VMA_MEMORY_USAGE_GPU_ONLY);
}

VkBool32 VKAPI_PTR RenderSystem_Vk::s_debugReport(
	VkDebugReportFlagsEXT flags, 
	VkDebugReportObjectTypeEXT objectType,
	uint64_t object,
	size_t location,
	int32_t messageCode,
	const char* pLayerPrefix,
	const char* pMessage,
	void* pUserData) 
{
	AX_LOG_ERROR("Vulkan error {}: {} - location={}", messageCode, StrView_c_str(pMessage), location);
	AX_ASSERT(false);
	return true;
}

} // namespace ax::AxRenderer
#endif // AX_RENDERER_VK