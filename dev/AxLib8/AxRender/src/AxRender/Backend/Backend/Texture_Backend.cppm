module;
export module AxRender:Texture_Backend;
export import :ImageIO;
export import :Texture;
export import :RenderObjectTable;

export namespace ax /*::AxRender*/ {

class Sampler_Backend : public  Sampler {
	AX_RTTI_INFO(Sampler_Backend, Sampler)
public:
	static SPtr<Sampler> s_new(const MemAllocRequest& req, const CreateDesc& desc);

	RenderObjectSlot<This>	objectSlot;

protected:
	Sampler_Backend(const CreateDesc& desc) : Base(desc), objectSlot(this, desc.isFallbackDefault) {}

	void _create(const CreateDesc& desc) { onCreate(desc); }
	virtual void onCreate(const CreateDesc& desc) {}
};

class Texture2D_Backend : public Texture2D  {
	AX_RTTI_INFO(Texture2D_Backend, Texture2D)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView assetPath);
	static SPtr<This> s_new(const MemAllocRequest& req, const ImageInfo& info, ByteSpan pixelData);
	static SPtr<This> s_new(const MemAllocRequest& req, const Image& image) { return s_new(req, image.info(), image.pixelData()); }

	void hotReloadFile();
	void hotCreateFromImage(const ImageInfo& info, ByteSpan pixelData);

	RenderObjectSlot<This>	objectSlot;

protected:
	Texture2D_Backend(const CreateDesc& desc) : Base(desc), objectSlot(this, desc.isFallbackDefault) {}

	void _loadFile();
	void _loadImage(const Image& image) { _loadImage(image.info(), image.pixelData()); }
	void _loadImage(const ImageInfo& info, ByteSpan pixelData);

	void _create(const CreateDesc& desc) { onCreate(desc); }
	virtual void onCreate(const CreateDesc& desc);

	void _onImageIO_ReadHandler(ImageIO_ReadHandler& handler) {
		objectSlot.markDirty();
		_info = handler.desc.info;
		onImageIO_ReadHandler(handler);
	}

	virtual void onImageIO_ReadHandler(ImageIO_ReadHandler& handler) = 0;
};

} // namespace