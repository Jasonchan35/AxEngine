module;
export module AxRender:Texture;
export import :RenderDataType;
export import :Image;

export namespace ax /*::AxRender*/ {

#define AX_RENDER_SamplerFilter_ENUM_LIST(E) \
	E(None,) \
	E(Point,)      /* 1 texels on closest mipmap level */ \
	E(Linear,)     /* 1 texels on 2 mipmap levels */ \
	E(Bilinear,)   /* 4 texels on closest mipmap level */  \
	E(Trilinear,)  /* 4 texels on 2 mipmap levels */\
	E(Anisotropic,) \
//----
AX_ENUM_CLASS(AX_RENDER_SamplerFilter_ENUM_LIST, SamplerFilter, u8)

#define AX_RENDER_SamplerWrap_EnumList(E) \
	E(None,) \
	E(Repeat,) \
	E(Clamp,) \
	E(Mirror,) \
	E(MirrorOnce,) \
//----
AX_ENUM_CLASS(AX_RENDER_SamplerWrap_EnumList, SamplerWrap, u8)

class SamplerWrapUVW {
	using T = SamplerWrap;
	static constexpr Int N = 3;
public:
	static constexpr Int kElementCount = N;

	SamplerWrap u, v, w;
	SamplerWrapUVW(SamplerWrap u_, SamplerWrap v_, SamplerWrap w_)
		: u(u_)
		, v(v_)
		, w(w_) {
	};

	SamplerWrapUVW(TagAll_T, SamplerWrap wrap)
		: u(wrap)
		, v(wrap)
		, w(wrap) {
	}

	void set(SamplerWrap u_, SamplerWrap v_, SamplerWrap w_) {
		u = u_;
		v = v_;
		w = w_;
	}

	void set(TagAll_T, SamplerWrap wrap) {
		u = wrap;
		v = wrap;
		w = wrap;
	}

	using CSpan =    Span<T>;
	using MSpan = MutSpan<T>;

	using CFixedSpan =    FixedSpan<T, N>;
	using MFixedSpan = MutFixedSpan<T, N>;

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(&u); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(&u); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }	
};

class SamplerState {
	using This = SamplerState;
public:
	SamplerState() {
		filter	= SamplerFilter::Bilinear;
		wrap	= {TagAll, SamplerWrap::Repeat};
	}

	union {
		struct {
			SamplerFilter	filter;
			SamplerWrapUVW	wrap;
		};
		u32 _packed;
	};
	
	float	minLOD	= 0;
	float	maxLOD	= Math::infinity();

	template<class SE>
	void onJsonIO(SE& se) {
		AX_JSON_IO(se, filter);
		AX_JSON_IO(se, wrap);
		AX_JSON_IO(se, minLOD);
		AX_JSON_IO(se, maxLOD);
	}

	HashInt onHashInt() const {
		static_assert(AX_SIZEOF(_packed) == AX_SIZEOF(filter) + AX_SIZEOF(wrap));
		u64 u = static_cast<u64>(_packed) ^ ax_bit_cast<u32>(minLOD) ^ ax_bit_cast<u32>(minLOD);
		return HashInt::s_make(u);
	}

	bool operator==(const This& r) const {
		return _packed == r._packed 
			&& Math::exactlyEqual(minLOD, r.minLOD)
			&& Math::exactlyEqual(maxLOD, r.maxLOD);
	}

	explicit operator bool() const { return filter != SamplerFilter::None; }
};

class Sampler_CreateDesc : NonCopyable {
public:
	SamplerState	samplerState;
};

class Sampler : public RenderObject {
	AX_RTTI_INFO(Sampler, RenderObject)
public:
	using CreateDesc = Sampler_CreateDesc;
	using ResourceKey = SamplerState;
	const ResourceKey& resourceKey() const { return _samplerState; }

	static SPtr<Sampler> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<Sampler> s_new(const MemAllocRequest& req, SamplerFilter filter, SamplerWrapUVW wrap) {
		CreateDesc desc;
		desc.samplerState.filter = filter;
		desc.samplerState.wrap   = wrap;
		return s_new(req, desc);
	}

	const SamplerState&	samplerState() const { return _samplerState; }

protected:
	Sampler(const CreateDesc& desc) : _samplerState(desc.samplerState) {}

	SamplerState	_samplerState;
};

class Texture : public RenderObject {
	AX_RTTI_INFO(Texture, RenderObject)
public:
	RenderDataType type() const { return _textureType; }

	using ResourceKey = String;
	const ResourceKey& resourceKey() const { return _assetPath; }

protected:
	Texture(RenderDataType type) : _textureType(type) {}
	RenderDataType _textureType = RenderDataType::None;

	String	_assetPath;
};

class Texture2D_CreateDesc : NonCopyable {
public:
	StrView			assetPath;
	ImageInfo		info;
	ByteSpan		pixelData;
	bool			isBackBuffer = false;
};

class Texture2D : public Texture {
	AX_RTTI_INFO(Texture2D, Texture)
public:
	using CreateDesc	= Texture2D_CreateDesc;

	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView assetPath);
	static SPtr<This> s_new(const MemAllocRequest& req, const ImageInfo& info, ByteSpan pixelData);
	static SPtr<This> s_new(const MemAllocRequest& req, const Image& image) { return s_new(req, image.info(), image.pixelData()); }

	ColorType	colorType	() const	{ return _info.colorType; }
	Int			mipLevels	() const	{ return _info.mipLevels; }
	Vec2i		size		() const	{ return _info.size.xy(); }

	const ImageInfo& info() const { return _info; }

friend class Texture2D_ImageIO_Reader;
protected:

	Texture2D(const CreateDesc& desc): Base(RenderDataType::Texture2D), _info(desc.info) {
		_assetPath = desc.assetPath;
	}

	
	ImageInfo	_info;
};

class Texture3D_CreateDesc : NonCopyable {
public:
	StrView			assetPath;
	ImageInfo		info;
};

class Texture3D : public Texture {
	AX_RTTI_INFO(Texture3D, Texture)
public:
	using CreateDesc = Texture3D_CreateDesc;

	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	
	Texture3D(const CreateDesc& desc) : Base(RenderDataType::Texture3D), _info(desc.info) {
		_assetPath = desc.assetPath;
	}


private:
	ImageInfo	_info;
};

class TextureCube_CreateDesc : NonCopyable {
public:
	StrView			assetPath;
	ImageInfo		info;
};

class TextureCube : public Texture {
	AX_RTTI_INFO(TextureCube, Texture)
public:
	using CreateDesc = TextureCube_CreateDesc;

	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	
	TextureCube(const CreateDesc& desc): Base(RenderDataType::TextureCube), _info(desc.info) {
		_assetPath = desc.assetPath;
	}

private:
	ImageInfo	_info;
};


template<> struct RenderDataType_get_<Sampler    > { static constexpr RenderDataType value = RenderDataType::SamplerState; };
template<> struct RenderDataType_get_<Texture2D  > { static constexpr RenderDataType value = RenderDataType::Texture2D;    };
template<> struct RenderDataType_get_<Texture3D  > { static constexpr RenderDataType value = RenderDataType::Texture3D;    };
template<> struct RenderDataType_get_<TextureCube> { static constexpr RenderDataType value = RenderDataType::TextureCube;  };


} // namespace