module;

export module AxRender:RenderSystem_Vk;

#if AX_RENDER_VK
export import :RenderSystem_Backend;
export import :AX_Vulkan;

export namespace ax /*::AxRender*/ {

class RenderSystem_Vk : public RenderSystem_Backend {
	AX_RTTI_INFO(RenderSystem_Vk, RenderSystem_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE()

	RenderSystem_Vk(const CreateDesc& desc);
	virtual ~RenderSystem_Vk() override;

	AX_VkPhysicalDeviceList& physicalDeviceList() { return _physicalDeviceList; }
	AX_VkDevice& device() { return _device; }

	struct DescriptorSets {
		AX_VkDescriptorSetLayout	frameLayout;
		AX_VkDescriptorSetLayout	renderPassLayout;
	};

	const DescriptorSets&	descriptorSets() { return _descriptorSets; }

protected:
	DescriptorSets	_descriptorSets;

	AX_RenderObject_LIST(AX_RenderSystem_NewObject, Vk, override)

private:
	void _createVkInstance();

	static VkBool32 VKAPI_PTR s_debugReport(
			VkDebugReportFlagsEXT      flags,
			VkDebugReportObjectTypeEXT objectType,
			uint64_t                   object,
			size_t                     location,
			int32_t                    messageCode,
			const char*                pLayerPrefix,
			const char*                pMessage,
			void*                      pUserData);

	AX_VkInstance				_vkInst;
	AX_VkPhysicalDeviceList		_physicalDeviceList;
	AX_VkDebugReportCallbackEXT	_debugReportCallbackExt;
	AX_VkDevice					_device;
};

} // namespace

#endif