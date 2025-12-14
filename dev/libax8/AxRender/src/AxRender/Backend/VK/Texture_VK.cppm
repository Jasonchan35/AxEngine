module;

export module AxRender:Texture_VK;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :Texture_Backend;
export import :GpuBuffer_Backend;

export namespace ax /*::AxRender*/ {

class Sampler_VK : public Sampler_Backend {
	AX_RTTI_INFO(Sampler_VK, Sampler_Backend)
public:
	Sampler_VK(const CreateDesc& desc) : Base(desc) {}

	VkSampler	vkHandle() { return _sampler.handle(); }

protected:
	virtual void onCreate(const CreateDesc& desc) override;
	AX_VkSampler		_sampler;
};

class Texture2D_VK : public Texture2D_Backend {
	AX_RTTI_INFO(Texture2D_VK, Texture2D_Backend)
public:
	Texture2D_VK(const CreateDesc& desc) : Base(desc) {}

	virtual void onImageIO_ReadHandler(ImageIO_ReadHandler& handler) override;

	void _bindImage(class RenderRequest_VK* req, VkDescriptorImageInfo& outInfo);
private:
	AX_VkImage			_image;
	AX_VkDeviceMemory	_devMem;
	AX_VkImageView		_view;

	SPtr<GpuBuffer_Backend>	_uploadBuffer;
};

} // namespace
#endif // AX_RENDERER_VK