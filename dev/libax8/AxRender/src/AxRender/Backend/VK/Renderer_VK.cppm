module;

export module AxRender:Renderer_VK;

#if AX_RENDERER_VK
export import :Renderer_Backend;
export import :AX_Vulkan;

export namespace ax::AxRender {

class Renderer_VK : public Renderer_Backend {
	AX_RTTI_INFO(Renderer_VK, Renderer_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE()

	Renderer_VK(const CreateDesc& desc);
	virtual ~Renderer_VK() override { destroy(); }

	AX_VkPhysicalDeviceList& physicalDeviceList() { return _physicalDeviceList; }
	AX_VkDevice& device() { return _device; }

	struct DescipterSets {
		AX_VkDescriptorSetLayout	frameLayout;
		AX_VkDescriptorSetLayout	renderPassLayout;
	};

	const DescipterSets&	descipterSets() { return _descipterSets; }

protected:
	DescipterSets	_descipterSets;

	AX_Renderer_FunctionInterfaces_override(VK)

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