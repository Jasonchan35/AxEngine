module;

export module AxRender:Texture_VK;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :Texture_Backend;
export import :GpuBuffer_Backend;

export namespace ax /*::AxRender*/ {

class Sampler_Vk : public Sampler_Backend {
	AX_RTTI_INFO(Sampler_Vk, Sampler_Backend)
public:
	Sampler_Vk(const CreateDesc& desc) : Base(desc) {}

	VkSampler	vkHandle() { return _sampler.handle(); }

protected:
	virtual void onCreate(const CreateDesc& desc) override;
	AX_VkSampler		_sampler;
};

class Texture2D_Vk : public Texture2D_Backend {
	AX_RTTI_INFO(Texture2D_Vk, Texture2D_Backend)
public:
	Texture2D_Vk(const CreateDesc& desc) : Base(desc) {}

	virtual void onImageIO_ReadHandler(ImageIO_ReadHandler& handler) override;

	void _bindImage(class RenderRequest_Vk* req, VkDescriptorImageInfo& outInfo);
private:
	AX_VkImage			_image;
	AX_VkDeviceMemory	_devMem;
	AX_VkImageView		_view;

	SPtr<GpuBuffer_Backend>	_uploadBuffer;
};

} // namespace
#endif // AX_RENDERER_VK