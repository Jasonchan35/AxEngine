module;

export module AxRender:Texture_Vk;

#if AX_RENDER_VK
export import :AX_Vulkan;
export import :Texture_Backend;
export import :GpuBuffer_Backend;
export import :RenderRequest_Vk;

export namespace ax /*::AxRender*/ {

class Sampler_Vk : public Sampler_Backend {
	AX_RTTI_INFO(Sampler_Vk, Sampler_Backend)
public:
	Sampler_Vk(const CreateDesc& desc) : Base(desc) {}

	VkSampler	vkHandle() { return _sampler.handle(); }

	VkDescriptorImageInfo _getUpdatedDescriptorInfo(RenderRequest_Vk* req) const {
		VkDescriptorImageInfo info = {};
		info.sampler = _sampler.handle();
		return info;
	}
protected:
	virtual void onCreate(const CreateDesc& desc) override;
	AX_VkSampler		_sampler;
};

class Texture2D_Vk : public Texture2D_Backend {
	AX_RTTI_INFO(Texture2D_Vk, Texture2D_Backend)
public:
	Texture2D_Vk(const CreateDesc& desc) : Base(desc) {}

	virtual void onImageIO_ReadHandler(ImageIO_ReadHandler& handler) override;

	VkDescriptorImageInfo _getUpdatedDescriptorInfo(class RenderRequest_Vk* req);
private:
	AX_VkImage			_image;
	AX_VkDeviceMemory	_devMem;
	AX_VkImageView		_view;

	SPtr<GpuBuffer_Backend>	_uploadBuffer;
};

} // namespace
#endif // AX_RENDER_VK