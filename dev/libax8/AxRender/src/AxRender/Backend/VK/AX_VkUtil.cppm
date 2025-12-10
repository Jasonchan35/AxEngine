module;

export module AxRender:AX_VkUtil;

#if AX_RENDERER_VK
export import :Shader;
export import :Texture;

namespace ax::AxRender {

class AX_VkUtil {
public:
	static VkShaderStageFlagBits	getVkShaderStageFlagBits	(ShaderStageFlags f);

	static VkPrimitiveTopology		getVkPrimitiveTopology	(PrimitiveType t);

	static VkIndexType				getVkIndexType			(IndexType t);
	static VkCompareOp				getVkDepthTestOp		(DepthTestOp v);

	static VkAttachmentLoadOp		getVkLoadOp				(BufferLoadOp v);

	static VkFormat					getVkColorType			(ColorType type);
	static VkFormat					getVkDataType			(DataType v);
	static VkFormat					getVkDepthType			(DepthType v);

	static VkFilter					getVkSamplerFilter		(SamplerFilter v);
	static VkSamplerAddressMode		getVkSamplerWrap		(SamplerWrap   v);
	static VkSamplerMipmapMode		getVkSamplerMipmapMode	(SamplerFilter v);

	static VkBlendOp				getVkBlendOp			(BlendOp		v);
	static VkBlendFactor			getVkBlendFactor		(BlendFactor	v);
	static VkCullModeFlagBits		getVkCullMode			(CullMode		v);

	static bool formatHasDepth	(VkFormat f);
	static bool formatHasStencil(VkFormat f);

	static uint32_t		castUInt32(Int v) 		{ return SafeCast(v); }
	static VkDeviceSize	castVkDeviceSize(Int v)	{ return SafeCast(v); }

	template<class R>
	static VkOffset2D	castVkOffset2D(const Vec2_<R>& v) {
		VkOffset2D o;
		o.x = SafeCast(v.x);
		o.y = SafeCast(v.y);
		return o;
	}

	template<class R>
	static VkExtent2D	castVkExtent2D(const Vec2_<R>& v) {
		VkExtent2D o;
		o.width  = SafeCast(v.x);
		o.height = SafeCast(v.y);
		return o;
	}

	template<class R>
	static VkRect2D		castVkRect2D(const Rect2_<R>& s) {
		VkRect2D o;
		o.offset = castVkOffset2D(s.pos);
		o.extent = castVkExtent2D(s.size);
		return o;
	}

AX_PRAGMA_GCC(diagnostic push)
AX_PRAGMA_GCC(diagnostic ignored "-Wold-style-cast")
	static u32 makeApiVersion(u32 variant, u32 major, u32 minor, u32 patch) { return VK_MAKE_API_VERSION(variant, major, minor, patch); }
	static u32 makeApiVersionMajor(u32 variant) { return VK_API_VERSION_MAJOR(variant); }
	static u32 makeApiVersionMinor(u32 variant) { return VK_API_VERSION_MINOR(variant); }
AX_PRAGMA_GCC(diagnostic pop)

	static void setFloat4(float d[4], const Color4f& s) {
AX_PRAGMA_GCC(diagnostic push)
AX_PRAGMA_GCC(diagnostic ignored "-Wunsafe-buffer-usage")
		d[0] = s.r; d[1] = s.g; d[2] = s.b; d[3] = s.a;
AX_PRAGMA_GCC(diagnostic pop)

	}

	template<class OBJ> constexpr static VkObjectType s_objectType();

	static class AX_VkAllocatorCallbacks* allocCallbacks();
	static bool checkResult(VkResult res) { return res == VK_SUCCESS; }
	static void throwIfError(VkResult res) { if (!checkResult(res)) throw Error_Undefined(); }
};

// In alphabet order
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkBuffer			>() { return VK_OBJECT_TYPE_BUFFER;	}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkCommandBuffer	>() { return VK_OBJECT_TYPE_COMMAND_BUFFER;	}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkCommandPool		>() { return VK_OBJECT_TYPE_COMMAND_POOL;	}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkDescriptorSet	>() { return VK_OBJECT_TYPE_DESCRIPTOR_SET;	}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkDevice			>() { return VK_OBJECT_TYPE_DEVICE;			}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkDeviceMemory	>() { return VK_OBJECT_TYPE_DEVICE_MEMORY;	}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkFence			>() { return VK_OBJECT_TYPE_FENCE;			}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkFramebuffer		>() { return VK_OBJECT_TYPE_FRAMEBUFFER;	}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkImage			>() { return VK_OBJECT_TYPE_IMAGE;			}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkImageView		>() { return VK_OBJECT_TYPE_IMAGE_VIEW;		}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkRenderPass		>() { return VK_OBJECT_TYPE_RENDER_PASS;	}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkSampler			>() { return VK_OBJECT_TYPE_SAMPLER;		}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkSemaphore		>() { return VK_OBJECT_TYPE_SEMAPHORE;		}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkShaderModule	>() { return VK_OBJECT_TYPE_SHADER_MODULE;	}
template<> constexpr VkObjectType AX_VkUtil::s_objectType<VkQueue			>() { return VK_OBJECT_TYPE_QUEUE;			}


class AX_VkAllocatorCallbacks : public VkAllocationCallbacks {
	using This = AX_VkAllocatorCallbacks;
	using Base = VkAllocationCallbacks;
public:
	AX_VkAllocatorCallbacks();

private:
	static void* s_alloc			(void* pUserData_,                  size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
	static void* s_realloc			(void* pUserData_, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
	static void  s_free				(void* pUserData_, void* pMemory);
	static void  s_allocNotification(void* pUserData_, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
	static void  s_freeNotification	(void* pUserData_, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);

	static This* s_getThis(void* userData) { return reinterpret_cast<This*>(userData); }

	MemAllocator*	_allocator = nullptr;
};

inline
VkShaderStageFlagBits AX_VkUtil::getVkShaderStageFlagBits(ShaderStageFlags f) {
	using SRC = ShaderStageFlags;
	u64 o = 0;
	if (EnumFn(f).hasFlags(SRC::Vertex		)) o |= VK_SHADER_STAGE_VERTEX_BIT;
	if (EnumFn(f).hasFlags(SRC::Pixel		)) o |= VK_SHADER_STAGE_FRAGMENT_BIT;
	if (EnumFn(f).hasFlags(SRC::Geometry	)) o |= VK_SHADER_STAGE_GEOMETRY_BIT;
	if (EnumFn(f).hasFlags(SRC::Compute		)) o |= VK_SHADER_STAGE_COMPUTE_BIT;
	return static_cast<VkShaderStageFlagBits>(o);
}

inline
VkPrimitiveTopology AX_VkUtil::getVkPrimitiveTopology(PrimitiveType t) {
	switch (t) {
		case PrimitiveType::Points:		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case PrimitiveType::Lines:		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case PrimitiveType::Triangles:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		default: throw Error_Undefined();
	}
}

inline
VkFormat AX_VkUtil::getVkColorType(ColorType type) {
	using SRC = ColorType;
	switch (type) {
		case SRC::HSVAf:	return VK_FORMAT_R32G32B32A32_SFLOAT;
		case SRC::RGBAf:	return VK_FORMAT_R32G32B32A32_SFLOAT;
		case SRC::RGBAh:	return VK_FORMAT_R16G16B16A16_SFLOAT;
		case SRC::RGBAb:	return VK_FORMAT_R8G8B8A8_UNORM;

//		case SRC::RGBb:
		case SRC::RGb:		return VK_FORMAT_R8G8_UNORM;
		case SRC::Rb:		return VK_FORMAT_R8_UNORM;
//		case SRC::Ab:		return VK_FORMAT_A8_UNORM;
		case SRC::Lb:		return VK_FORMAT_R8_UNORM;
		case SRC::Af:		return VK_FORMAT_R32_SFLOAT;
		case SRC::Lf:		return VK_FORMAT_R32_SFLOAT;
		case SRC::LAf:		return VK_FORMAT_R32G32_SFLOAT;
		case SRC::LAb:		return VK_FORMAT_R8G8_UNORM;
//
		case SRC::DXT_BC1:	return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case SRC::DXT_BC2:	return VK_FORMAT_BC2_UNORM_BLOCK;
		case SRC::DXT_BC3:	return VK_FORMAT_BC3_UNORM_BLOCK;
		case SRC::DXT_BC4:	return VK_FORMAT_BC4_UNORM_BLOCK;
		case SRC::DXT_BC5:	return VK_FORMAT_BC5_UNORM_BLOCK;
		case SRC::DXT_BC6h:	return VK_FORMAT_BC6H_UFLOAT_BLOCK;
		case SRC::DXT_BC7:	return VK_FORMAT_BC7_UNORM_BLOCK;
		default: throw Error_Undefined();
	}
}

inline
VkFormat AX_VkUtil::getVkDataType(DataType v) {
	using SRC = DataType;
	switch (v) {
//---- UInt8 ----
		case SRC::u8:			return VK_FORMAT_R8_UINT;
		case SRC::u8x2:			return VK_FORMAT_R8G8_UINT;
		case SRC::u8x3:			return VK_FORMAT_R8G8B8_UINT;
		case SRC::u8x4:			return VK_FORMAT_R8G8B8A8_UINT;
//---- UInt16 ----
		case SRC::u16:			return VK_FORMAT_R16_UINT;
		case SRC::u16x2:		return VK_FORMAT_R16G16_UINT;
		case SRC::u16x3:		return VK_FORMAT_R16G16B16_UINT;
		case SRC::u16x4:		return VK_FORMAT_R16G16B16A16_UINT;
//---- UInt32 ----
		case SRC::u32:			return VK_FORMAT_R32_UINT;
		case SRC::u32x2:		return VK_FORMAT_R32G32_UINT;
		case SRC::u32x3:		return VK_FORMAT_R32G32B32_UINT;
		case SRC::u32x4:		return VK_FORMAT_R32G32B32A32_UINT;
//---- Int8 ----
		case SRC::i8:			return VK_FORMAT_R8_SINT;
		case SRC::i8x2:			return VK_FORMAT_R8G8_SINT;
		case SRC::i8x3:			return VK_FORMAT_R8G8B8_SINT;
		case SRC::i8x4:			return VK_FORMAT_R8G8B8A8_SINT;
//---- Int16 ----
		case SRC::i16:			return VK_FORMAT_R16_SINT;
		case SRC::i16x2:		return VK_FORMAT_R16G16_SINT;
		case SRC::i16x3:		return VK_FORMAT_R16G16B16_SINT;
		case SRC::i16x4:		return VK_FORMAT_R16G16B16A16_SINT;
//---- Int32 ----
		case SRC::i32:			return VK_FORMAT_R32_SINT;
		case SRC::i32x2:		return VK_FORMAT_R32G32_SINT;
		case SRC::i32x3:		return VK_FORMAT_R32G32B32_SINT;
		case SRC::i32x4:		return VK_FORMAT_R32G32B32A32_SINT;
//---- UNorm8 ----
		case SRC::UNorm8:		return VK_FORMAT_R8_UNORM;
		case SRC::UNorm8x2:		return VK_FORMAT_R8G8_UNORM;
		case SRC::UNorm8x3:		return VK_FORMAT_R8G8B8_UNORM;
		case SRC::UNorm8x4:		return VK_FORMAT_R8G8B8A8_UNORM;
//---- UNorm16 ----
		case SRC::UNorm16:		return VK_FORMAT_R16_UNORM;
		case SRC::UNorm16x2:	return VK_FORMAT_R16G16_UNORM;
		case SRC::UNorm16x3:	return VK_FORMAT_R16G16B16_UNORM;
		case SRC::UNorm16x4:	return VK_FORMAT_R16G16B16A16_UNORM;
//---- SNorm8 ----
		case SRC::SNorm8:		return VK_FORMAT_R8_SNORM;
		case SRC::SNorm8x2:		return VK_FORMAT_R8G8_SNORM;
		case SRC::SNorm8x3:		return VK_FORMAT_R8G8B8_SNORM;
		case SRC::SNorm8x4:		return VK_FORMAT_R8G8B8A8_SNORM;
//---- SNorm16 ----
		case SRC::SNorm16:		return VK_FORMAT_R16_SNORM;
		case SRC::SNorm16x2:	return VK_FORMAT_R16G16_SNORM;
		case SRC::SNorm16x3:	return VK_FORMAT_R16G16B16_SNORM;
		case SRC::SNorm16x4:	return VK_FORMAT_R16G16B16A16_SNORM;
//---- Float16 ----
		case SRC::f16:			return VK_FORMAT_R16_SFLOAT;
		case SRC::f16x2:		return VK_FORMAT_R16G16_SFLOAT;
		case SRC::f16x3:		return VK_FORMAT_R16G16B16_SFLOAT;
		case SRC::f16x4:		return VK_FORMAT_R16G16B16A16_SFLOAT;
//---- Float32 ----
		case SRC::f32:			return VK_FORMAT_R32_SFLOAT;
		case SRC::f32x2:		return VK_FORMAT_R32G32_SFLOAT;
		case SRC::f32x3:		return VK_FORMAT_R32G32B32_SFLOAT;
		case SRC::f32x4:		return VK_FORMAT_R32G32B32A32_SFLOAT;
//---- Float64 ----
		case SRC::f64:			return VK_FORMAT_R64_SFLOAT;
		case SRC::f64x2:		return VK_FORMAT_R64G64_SFLOAT;
		case SRC::f64x3:		return VK_FORMAT_R64G64B64_SFLOAT;
		case SRC::f64x4:		return VK_FORMAT_R64G64B64A64_SFLOAT;
//----------
		case SRC::Color3b:		return VK_FORMAT_R8G8B8_UNORM;
		case SRC::Color3s:		return VK_FORMAT_R16G16B16_UNORM;
		case SRC::Color3h:		return VK_FORMAT_R16G16B16_SFLOAT;
		case SRC::Color3f:		return VK_FORMAT_R32G32B32_SFLOAT;
		case SRC::Color3d:		return VK_FORMAT_R64G64B64_SFLOAT;
//----------
		case SRC::Color4b:		return VK_FORMAT_R8G8B8A8_UNORM;
		case SRC::Color4s:		return VK_FORMAT_R16G16B16A16_UNORM;
		case SRC::Color4h:		return VK_FORMAT_R16G16B16A16_SFLOAT;
		case SRC::Color4f:		return VK_FORMAT_R32G32B32A32_SFLOAT;
		case SRC::Color4d:		return VK_FORMAT_R64G64B64A64_SFLOAT;
//---
		default: throw Error_Undefined();
	}
}

inline
VkFormat AX_VkUtil::getVkDepthType(DepthType v) {
	using SRC = DepthType;
	switch (v) {
		case SRC::Depth_Float32:				return VK_FORMAT_D32_SFLOAT;
		case SRC::Depth_Float32_Stencil_UInt8:	return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case SRC::Depth_UNorm16:				return VK_FORMAT_D16_UNORM;
		case SRC::Depth_Unorm16_Stencil_UInt8:	return VK_FORMAT_D16_UNORM_S8_UINT;
		case SRC::Depth_Unorm24_Stencil_UInt8:	return VK_FORMAT_D24_UNORM_S8_UINT;
		default: throw Error_Undefined();
	}
}


inline VkFilter AX_VkUtil::getVkSamplerFilter(SamplerFilter v) {
	using SRC = SamplerFilter;
	switch (v) {
		case SRC::Point:		return VK_FILTER_NEAREST;
		case SRC::Linear:		return VK_FILTER_NEAREST;
		case SRC::Bilinear:		return VK_FILTER_LINEAR;
		case SRC::Trilinear:	return VK_FILTER_LINEAR;
		case SRC::Anisotropic:	return VK_FILTER_LINEAR;
	//---
		default: throw Error_Undefined();
	}
}

inline VkSamplerMipmapMode AX_VkUtil::getVkSamplerMipmapMode(SamplerFilter v) {
	using SRC = SamplerFilter;
	switch (v) {
		case SRC::Point:		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case SRC::Linear:		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		case SRC::Bilinear:		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case SRC::Trilinear:	return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		case SRC::Anisotropic:	return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	//---
		default: throw Error_Undefined();
	}
}

inline VkSamplerAddressMode AX_VkUtil::getVkSamplerWrap(SamplerWrap v) {
	using SRC = SamplerWrap;
	switch (v) {
		case SRC::Repeat:		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case SRC::Clamp:		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case SRC::Mirror:		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case SRC::MirrorOnce:	return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
	//---
		default: throw Error_Undefined();
	}
}

inline VkBlendOp AX_VkUtil::getVkBlendOp(BlendOp v) {
	using SRC = BlendOp;
	switch (v) {
		case SRC::Add:		return VK_BLEND_OP_ADD;
		case SRC::Sub:		return VK_BLEND_OP_SUBTRACT;
		case SRC::RevSub:	return VK_BLEND_OP_REVERSE_SUBTRACT;
		case SRC::Min:		return VK_BLEND_OP_MIN;
		case SRC::Max:		return VK_BLEND_OP_MAX;
		default: throw Error_Undefined();
	}
}

inline VkBlendFactor AX_VkUtil::getVkBlendFactor(BlendFactor v) {
	using SRC = BlendFactor;
	switch (v) {
		case SRC::Zero:						return VK_BLEND_FACTOR_ZERO;
		case SRC::One:						return VK_BLEND_FACTOR_ONE;
		case SRC::SrcAlpha:					return VK_BLEND_FACTOR_SRC_ALPHA;
		case SRC::DstAlpha:					return VK_BLEND_FACTOR_DST_ALPHA;
		case SRC::SrcColor:					return VK_BLEND_FACTOR_SRC_COLOR;
		case SRC::DstColor:					return VK_BLEND_FACTOR_DST_COLOR;
		case SRC::ConstColor:				return VK_BLEND_FACTOR_CONSTANT_COLOR;
			//	case SRC::ConstAlpha:				return VK_BLEND_FACTOR_CONSTANT_ALPHA;
		case SRC::OneMinusSrcAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case SRC::OneMinusSrcColor:			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case SRC::OneMinusDstAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case SRC::OneMinusDstColor:			return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case SRC::OneMinusConstColor:		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
			//	case SRC::OneMinusConstAlpha:		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
		case SRC::SrcAlphaSaturate:			return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		default: throw Error_Undefined();
	}
}

inline VkCullModeFlagBits AX_VkUtil::getVkCullMode(CullMode v) {
	using SRC = CullMode;
	switch (v) {
		case SRC::None:		return VK_CULL_MODE_NONE;
		case SRC::Back:		return VK_CULL_MODE_BACK_BIT;
		case SRC::Front:	return VK_CULL_MODE_FRONT_BIT;
		default: throw Error_Undefined();
	}
}

inline
VkIndexType AX_VkUtil::getVkIndexType(IndexType t) {
	using SRC = IndexType;
	switch (t) {
		case SRC::UInt16:	return VK_INDEX_TYPE_UINT16;
		case SRC::UInt32:	return VK_INDEX_TYPE_UINT32;
		default: throw Error_Undefined();
	}
}

inline
VkCompareOp AX_VkUtil::getVkDepthTestOp(DepthTestOp v) {
	using SRC = DepthTestOp;
	switch (v) {
		case SRC::Always:		return  VK_COMPARE_OP_ALWAYS;
		case SRC::Less:			return  VK_COMPARE_OP_LESS;
		case SRC::Equal:		return  VK_COMPARE_OP_EQUAL;
		case SRC::LessEqual:	return  VK_COMPARE_OP_LESS_OR_EQUAL;
		case SRC::Greater:		return  VK_COMPARE_OP_GREATER;
		case SRC::GreaterEqual:	return  VK_COMPARE_OP_GREATER_OR_EQUAL;
		case SRC::NotEqual:		return  VK_COMPARE_OP_NOT_EQUAL;
		case SRC::Never:		return  VK_COMPARE_OP_NEVER;
		default: throw Error_Undefined();
	}
}

inline
VkAttachmentLoadOp ax::AxRender::AX_VkUtil::getVkLoadOp(BufferLoadOp v) {
	using SRC = BufferLoadOp;
	switch (v) {
		case SRC::DontCare:		return  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		case SRC::Load:			return  VK_ATTACHMENT_LOAD_OP_LOAD;
		case SRC::Clear:		return  VK_ATTACHMENT_LOAD_OP_CLEAR;
		default: throw Error_Undefined();
	}
}

} // namespace ax::AxRender

#endif // AX_RENDERER_VK