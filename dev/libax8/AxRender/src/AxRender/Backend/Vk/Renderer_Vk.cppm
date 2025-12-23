module;

export module AxRender:Renderer_Vk;

#if AX_RENDERER_VK
export import :Renderer_Backend;
export import :AX_Vulkan;

export namespace ax /*::AxRender*/ {

class Renderer_Vk : public Renderer_Backend {
	AX_RTTI_INFO(Renderer_Vk, Renderer_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE()

	Renderer_Vk(const CreateDesc& desc);
	virtual ~Renderer_Vk() override { destroy(); }

	AX_VkPhysicalDeviceList& physicalDeviceList() { return _physicalDeviceList; }
	AX_VkDevice& device() { return _device; }

	struct DescriptorSets {
		AX_VkDescriptorSetLayout	frameLayout;
		AX_VkDescriptorSetLayout	renderPassLayout;
	};

	const DescriptorSets&	descriptorSets() { return _descriptorSets; }

protected:
	DescriptorSets	_descriptorSets;

	AX_Renderer_FunctionInterfaces(Vk, override)

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