module;

export module AxRender:Dx12Util;

#if AX_RENDERER_DX12
export import :Shader;
export import :Texture;
export import :RenderCommandList;

namespace ax /*::AxRender*/ {

class RenderSystem_Dx12;

using AX_IDXGIFactory    = IDXGIFactory6;
using AX_IDXGIAdapter    = IDXGIAdapter4;
using AX_IDXGISwapChain  = IDXGISwapChain4;

using AX_ID3D12Device              = ID3D12Device9;
using AX_ID3D12Heap                = ID3D12Heap1;
using AX_ID3D12Resource            = ID3D12Resource2;
using AX_ID3D12GraphicsCommandList = ID3D12GraphicsCommandList6;
using AX_ID3D12CommandAllocator    = ID3D12CommandAllocator;

#if _DEBUG
using AX_IDXGIDebug  = IDXGIDebug1;
using AX_ID3D12Debug = ID3D12Debug6;
#endif

struct Dx12Util {
	Dx12Util() = delete;

	class DxResourceCreator;

	static constexpr bool isValid		(HRESULT hr) { if (!checkError(hr)) { reportError(hr); return false; } return true; }

	static constexpr void throwIfError	(HRESULT hr) { if (!checkError(hr)) { reportError(hr); throw Error_Undefined(); } }
	static constexpr void throwIfError	(HRESULT hr, ID3DBlob* error);

	static constexpr void assertIfError	(HRESULT hr) { if (!checkError(hr)) { reportError(hr); AX_ASSERT(false); } }
	static           void reportError	(HRESULT hr);

	static constexpr bool checkError	(HRESULT hr) { return SUCCEEDED(hr); }
	
	static constexpr D3D12_PRIMITIVE_TOPOLOGY		getDxPrimitiveTopology		(RenderPrimitiveType t);
	static constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE	getDxPrimitiveTopologyType	(RenderPrimitiveType t);
	static constexpr D3D12_FILTER					getDxSamplerFilter			(SamplerFilter v);
	static constexpr D3D12_TEXTURE_ADDRESS_MODE		getDxSamplerWrap			(SamplerWrap v);
	static constexpr DXGI_FORMAT					getDxIndexType				(VertexIndexType t);
	static constexpr Int							getDxIndexStrideInBytes		(VertexIndexType t);
	static constexpr D3D12_COMPARISON_FUNC			getDxDepthTestOp			(RenderState_DepthTestOp v);

	static constexpr DXGI_FORMAT		getDxColorType		(ColorType type);
	static constexpr DXGI_FORMAT		getDxDepthType		(RenderDepthType type);
	static constexpr DXGI_FORMAT		getDxDataType		(RenderDataType     v);
	static constexpr D3D12_BLEND_OP		getDxBlendOp		(RenderState_BlendOp		v);
	static constexpr D3D12_BLEND		getDxBlendFactor	(RenderState_BlendFactor	v);
	static constexpr D3D12_CULL_MODE	getDxCullMode		(RenderState_CullMode		v);

	static constexpr D3D12_COMMAND_LIST_TYPE getDxCommandListType(RenderCommandListType type);

	static D3D12_SHADER_BYTECODE getDxBytecode(ByteSpan span) {
		return {.pShaderBytecode = span.data(), .BytecodeLength = ax_safe_cast_from(span.sizeInBytes())};
	} 
	
	static constexpr StrView	errorToStrView(ID3DBlob* blob);

	static constexpr UINT		castUINT	(Int v) { return ax_safe_cast_from(v); }
	static constexpr UINT16		castUINT16	(Int v) { return ax_safe_cast_from(v); }
	static constexpr UINT64		castUINT64	(Int v) { return ax_safe_cast_from(v); }
	static constexpr DWORD		castDWORD	(Int v) { return ax_safe_cast_from(v); }
};

constexpr void Dx12Util::throwIfError(HRESULT hr, ID3DBlob* error) {
	if (!checkError(hr)) {
		StrView msg;
		if (error) {
			msg = errorToStrView(error);
			AX_LOG("DX12 error: {}", msg);
		}
		throw Error_Undefined(msg);
	}
}

constexpr D3D12_PRIMITIVE_TOPOLOGY Dx12Util::getDxPrimitiveTopology(RenderPrimitiveType t) {
	using SRC = RenderPrimitiveType;
	switch (t) {
		case SRC::Points:		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case SRC::Lines:		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case SRC::Triangles:	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		default: throw Error_Undefined();
	}
}

constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE Dx12Util::getDxPrimitiveTopologyType(RenderPrimitiveType t) {
	using SRC = RenderPrimitiveType;
	switch (t) {
		case SRC::Points:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case SRC::Lines:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case SRC::Triangles:	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		default: throw Error_Undefined();
	}
}

constexpr D3D12_FILTER Dx12Util::getDxSamplerFilter(SamplerFilter v) {
	using SRC = SamplerFilter;
	switch (v) {
		case SRC::Point:		return D3D12_FILTER_MIN_MAG_MIP_POINT;
		case SRC::Linear:		return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case SRC::Bilinear:		return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case SRC::Trilinear:	return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		case SRC::Anisotropic:	return D3D12_FILTER_ANISOTROPIC;
	//---
		default: throw Error_Undefined();
	}
}

constexpr D3D12_TEXTURE_ADDRESS_MODE Dx12Util::getDxSamplerWrap(SamplerWrap v) {
	using SRC = SamplerWrap;
	switch (v) {
		case SRC::Repeat:		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case SRC::Clamp:		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case SRC::Mirror:		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case SRC::MirrorOnce:	return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
	//---
		default: throw Error_Undefined();
	}
}

constexpr DXGI_FORMAT Dx12Util::getDxIndexType(VertexIndexType t) {
	using SRC = VertexIndexType;
	switch (t) {
		case SRC::u16:	return DXGI_FORMAT_R16_UINT;
		case SRC::u32:	return DXGI_FORMAT_R32_UINT;
		default: throw Error_Undefined();
	}
}

constexpr Int Dx12Util::getDxIndexStrideInBytes(VertexIndexType t) {
	using SRC = VertexIndexType;
	switch (t) {
		case SRC::u16:	return AX_SIZEOF(u16);
		case SRC::u32:	return AX_SIZEOF(u32);
		default: throw Error_Undefined();
	}
}

constexpr StrView Dx12Util::errorToStrView(ID3DBlob* blob) {
	if (blob == nullptr)
		return StrView();
	auto* s = static_cast<const char*>(blob->GetBufferPointer());
	auto  n = static_cast<Int>(blob->GetBufferSize());
	return StrView(s, n);
}

constexpr DXGI_FORMAT Dx12Util::getDxColorType(ColorType type) {
	using SRC = ColorType;
	switch (type) {
		case SRC::R11G11B10:	return DXGI_FORMAT_R11G11B10_FLOAT;
		case SRC::R10G10B10A2:	return DXGI_FORMAT_R10G10B10A2_UNORM;
//----		
		case SRC::HSVAf: 	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case SRC::RGBAf: 	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case SRC::RGBAh: 	return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case SRC::RGBAb: 	return DXGI_FORMAT_R8G8B8A8_UNORM;
//		case SRC::RGBb:
		case SRC::RGb:	 	return DXGI_FORMAT_R8G8_UNORM;
		case SRC::Rb:	 	return DXGI_FORMAT_R8_UNORM;
		case SRC::Ab:	 	return DXGI_FORMAT_A8_UNORM;
		case SRC::Lb:	 	return DXGI_FORMAT_R8_UNORM;
		case SRC::Af:	 	return DXGI_FORMAT_R32_FLOAT;
		case SRC::Lf:	 	return DXGI_FORMAT_R32_FLOAT;
		case SRC::LAf:	 	return DXGI_FORMAT_R32G32_FLOAT;
		case SRC::LAb:	 	return DXGI_FORMAT_R8G8_UNORM;
//
		case SRC::DXT_BC1:	return DXGI_FORMAT_BC1_UNORM;
		case SRC::DXT_BC2:	return DXGI_FORMAT_BC2_UNORM;
		case SRC::DXT_BC3:	return DXGI_FORMAT_BC3_UNORM;
		case SRC::DXT_BC4:	return DXGI_FORMAT_BC4_UNORM;
		case SRC::DXT_BC5:	return DXGI_FORMAT_BC5_UNORM;
		case SRC::DXT_BC6h:	return DXGI_FORMAT_BC6H_UF16;
		case SRC::DXT_BC7:	return DXGI_FORMAT_BC7_UNORM;
//
		default: throw Error_Undefined();
	}
}

constexpr DXGI_FORMAT Dx12Util::getDxDepthType(RenderDepthType type) {
	using SRC = RenderDepthType;
	switch (type) {
		case SRC::Depth_Float32:				return DXGI_FORMAT_D32_FLOAT;
		case SRC::Depth_Float32_Stencil_UInt8:	return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case SRC::Depth_UNorm16:				return DXGI_FORMAT_D16_UNORM;
//		case SRC::Depth_UNorm16_Stencil_UInt8:	return DXGI_FORMAT_D16_UNORM_S8_UINT;
		case SRC::Depth_UNorm24_Stencil_UInt8:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
//		case SRC::Stencil_UInt8:				return DXGI_FORMAT_S8_UINT;
		default: throw Error_Undefined();
	}
}

constexpr DXGI_FORMAT Dx12Util::getDxDataType(RenderDataType v) {
	using SRC = RenderDataType;
	switch (v) {
//---- UInt8 ----
		case SRC::u8:			return DXGI_FORMAT_R8_UINT;
		case SRC::u8x2:			return DXGI_FORMAT_R8G8_UINT;
//		case SRC::u8x3:			return DXGI_FORMAT_R8G8B8_UINT;			// not support in DX12
		case SRC::u8x4:			return DXGI_FORMAT_R8G8B8A8_UINT;
//---- UInt16 ----
		case SRC::u16:			return DXGI_FORMAT_R16_UINT;
		case SRC::u16x2:		return DXGI_FORMAT_R16G16_UINT;
//		case SRC::u16x3:		return DXGI_FORMAT_R16G16B16_UINT;		// not support in DX12
		case SRC::u16x4:		return DXGI_FORMAT_R16G16B16A16_UINT;
//---- UInt32 ----
		case SRC::u32:			return DXGI_FORMAT_R32_UINT;
		case SRC::u32x2:		return DXGI_FORMAT_R32G32_UINT;
		case SRC::u32x3:		return DXGI_FORMAT_R32G32B32_UINT;
		case SRC::u32x4:		return DXGI_FORMAT_R32G32B32A32_UINT;
//---- Int8 ----
		case SRC::i8:			return DXGI_FORMAT_R8_SINT;
		case SRC::i8x2:			return DXGI_FORMAT_R8G8_SINT;
//		case SRC::i8x3:			return DXGI_FORMAT_R8G8B8_SINT;			// not support in DX12
		case SRC::i8x4:			return DXGI_FORMAT_R8G8B8A8_SINT;
//---- Int16 ----
		case SRC::i16:			return DXGI_FORMAT_R16_SINT;
		case SRC::i16x2:		return DXGI_FORMAT_R16G16_SINT;
//		case SRC::i16x3:		return DXGI_FORMAT_R16G16B16_SINT;		// not support in DX12
		case SRC::i16x4:		return DXGI_FORMAT_R16G16B16A16_SINT;
//---- Int32 ----
		case SRC::i32:			return DXGI_FORMAT_R32_SINT;
		case SRC::i32x2:		return DXGI_FORMAT_R32G32_SINT;
		case SRC::i32x3:		return DXGI_FORMAT_R32G32B32_SINT;
		case SRC::i32x4:		return DXGI_FORMAT_R32G32B32A32_SINT;
//---- UNorm8 ----
		case SRC::UNorm8:		return DXGI_FORMAT_R8_UNORM;
		case SRC::UNorm8x2:		return DXGI_FORMAT_R8G8_UNORM;
//		case SRC::UNorm8x3:		return DXGI_FORMAT_R8G8B8_UNORM;		// not support in DX12  
		case SRC::UNorm8x4:		return DXGI_FORMAT_R8G8B8A8_UNORM;
//---- UNorm16 ----
		case SRC::UNorm16:		return DXGI_FORMAT_R16_UNORM;
		case SRC::UNorm16x2:	return DXGI_FORMAT_R16G16_UNORM;
//		case SRC::UNorm16x3:	return DXGI_FORMAT_R16G16B16_UNORM;		// not support in DX12   
		case SRC::UNorm16x4:	return DXGI_FORMAT_R16G16B16A16_UNORM;
//---- SNorm8 ----
		case SRC::SNorm8:		return DXGI_FORMAT_R8_SNORM;
		case SRC::SNorm8x2:		return DXGI_FORMAT_R8G8_SNORM;
//		case SRC::SNorm8x3:		return DXGI_FORMAT_R8G8B8_SNORM;		// not support in DX12
		case SRC::SNorm8x4:		return DXGI_FORMAT_R8G8B8A8_SNORM;
//---- SNorm16 ----
		case SRC::SNorm16:		return DXGI_FORMAT_R16_SNORM;
		case SRC::SNorm16x2:	return DXGI_FORMAT_R16G16_SNORM;
//		case SRC::SNorm16x3:	return DXGI_FORMAT_R16G16B16_USORM;		// not support in DX12
		case SRC::SNorm16x4:	return DXGI_FORMAT_R16G16B16A16_SNORM;
//---- Float16 ----
		case SRC::f16:			return DXGI_FORMAT_R16_FLOAT;
		case SRC::f16x2:		return DXGI_FORMAT_R16G16_FLOAT;
//		case SRC::f16x3:		return DXGI_FORMAT_R16G16B16_UFLOAT;	// not support in DX12
		case SRC::f16x4:		return DXGI_FORMAT_R16G16B16A16_FLOAT;
//---- Float32 ----
		case SRC::f32:			return DXGI_FORMAT_R32_FLOAT;
		case SRC::f32x2:		return DXGI_FORMAT_R32G32_FLOAT;
		case SRC::f32x3:		return DXGI_FORMAT_R32G32B32_FLOAT;
		case SRC::f32x4:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
//---- Float64 ----
//		case SRC::f64:			return DXGI_FORMAT_R64_FLOAT;
//		case SRC::f64x2:		return DXGI_FORMAT_R64G64_FLOAT;
//		case SRC::f64x3:		return DXGI_FORMAT_R64G64B64_FLOAT;
//		case SRC::f64x4:		return DXGI_FORMAT_R64G64B64A64_FLOAT;
//---- Color3 ------
//		case SRC::Color3b:		return DXGI_FORMAT_R8G8B8_UNORM;
//		case SRC::Color3s:		return DXGI_FORMAT_R16G16B16_UNORM;
//		case SRC::Color3h:		return DXGI_FORMAT_R16G16B16_SFLOAT;
//		case SRC::Color3f:		return DXGI_FORMAT_R32G32B32_SFLOAT;
//		case SRC::Color3d:		return DXGI_FORMAT_R64G64B64_SFLOAT;
//---- Color4 ------
		case SRC::Color4b:		return DXGI_FORMAT_R8G8B8A8_UNORM;
		case SRC::Color4s:		return DXGI_FORMAT_R16G16B16A16_UNORM;
		case SRC::Color4h:		return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case SRC::Color4f:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
//		case SRC::Color4d:		return DXGI_FORMAT_R64G64B64A64_FLOAT;
//---
		default: throw Error_Undefined();
	}
}

constexpr D3D12_BLEND_OP Dx12Util::getDxBlendOp(RenderState_BlendOp v) {
	using SRC = RenderState_BlendOp;
	switch (v) {
		case SRC::Add:		return D3D12_BLEND_OP_ADD;
		case SRC::Sub:		return D3D12_BLEND_OP_SUBTRACT;
		case SRC::RevSub:	return D3D12_BLEND_OP_REV_SUBTRACT;
		case SRC::Min:		return D3D12_BLEND_OP_MIN;
		case SRC::Max:		return D3D12_BLEND_OP_MAX;
		default: throw Error_Undefined();
	}
}

constexpr D3D12_BLEND Dx12Util::getDxBlendFactor(RenderState_BlendFactor v) {
	using SRC = RenderState_BlendFactor;
	switch (v) {
		case SRC::Zero:						return D3D12_BLEND_ZERO;
		case SRC::One:						return D3D12_BLEND_ONE;
		case SRC::SrcAlpha:					return D3D12_BLEND_SRC_ALPHA;
		case SRC::DstAlpha:					return D3D12_BLEND_DEST_ALPHA;
		case SRC::SrcColor:					return D3D12_BLEND_SRC_COLOR;
		case SRC::DstColor:					return D3D12_BLEND_DEST_COLOR;
		case SRC::ConstColor:				return D3D12_BLEND_BLEND_FACTOR;
//		case SRC::ConstAlpha:				return
		case SRC::OneMinusSrcAlpha:			return D3D12_BLEND_INV_SRC_ALPHA;
		case SRC::OneMinusSrcColor:			return D3D12_BLEND_INV_SRC_COLOR;
		case SRC::OneMinusDstAlpha:			return D3D12_BLEND_INV_DEST_ALPHA;
		case SRC::OneMinusDstColor:			return D3D12_BLEND_INV_DEST_COLOR;
		case SRC::OneMinusConstColor:		return D3D12_BLEND_INV_BLEND_FACTOR;
//		case SRC::OneMinusConstAlpha:		return
		case SRC::SrcAlphaSaturate:			return D3D12_BLEND_SRC_ALPHA_SAT;
		default: throw Error_Undefined();
	}
}

constexpr D3D12_CULL_MODE Dx12Util::getDxCullMode(RenderState_CullMode v) {
	using SRC = RenderState_CullMode;
	switch (v) {
		case SRC::None:	return D3D12_CULL_MODE_NONE;
		case SRC::Back:	return D3D12_CULL_MODE_BACK;
		case SRC::Front:	return D3D12_CULL_MODE_FRONT;
		default: throw Error_Undefined();
	}
}

constexpr D3D12_COMMAND_LIST_TYPE Dx12Util::getDxCommandListType(RenderCommandListType type) {
	using SRC = RenderCommandListType;
	switch (type) {
		case SRC::None:			return D3D12_COMMAND_LIST_TYPE_NONE;
		case SRC::Direct:		return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case SRC::Bundle:		return D3D12_COMMAND_LIST_TYPE_BUNDLE;
		case SRC::Compute:		return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case SRC::Copy:			return D3D12_COMMAND_LIST_TYPE_COPY;
		case SRC::VideoDecode:	return D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE;
		case SRC::VideoProcess:	return D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS;
		case SRC::VideoEncode:	return D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE;
		default: throw Error_Undefined();
	}
}

constexpr D3D12_COMPARISON_FUNC Dx12Util::getDxDepthTestOp(RenderState_DepthTestOp v) {
	using SRC = RenderState_DepthTestOp;
	switch (v) {
		case SRC::Always:		return  D3D12_COMPARISON_FUNC_ALWAYS;
		case SRC::Less:			return  D3D12_COMPARISON_FUNC_LESS;
		case SRC::Equal:		return  D3D12_COMPARISON_FUNC_EQUAL;
		case SRC::LessEqual:	return  D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case SRC::Greater:		return  D3D12_COMPARISON_FUNC_GREATER;
		case SRC::GreaterEqual:	return  D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case SRC::NotEqual:		return  D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case SRC::Never:		return  D3D12_COMPARISON_FUNC_NEVER;
		default: throw Error_Undefined();
	}
}

template<>
constexpr StrLit ax_enum_entry_strlit(const D3D12_DESCRIPTOR_HEAP_TYPE& v) {
	switch (v) {
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV  : return "CBV_SRV_UAV";
		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER      : return "SAMPLER"    ;
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV          : return "RTV"        ;
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV          : return "DSV"        ;
		case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES    : return "NUM_TYPES"  ;
		default: return "";
	}
}

template<>
constexpr StrLit ax_enum_entry_strlit(const D3D12_DESCRIPTOR_RANGE_TYPE& v) {
	switch (v) {
		case D3D12_DESCRIPTOR_RANGE_TYPE_SRV        : return "SRV"    ;
		case D3D12_DESCRIPTOR_RANGE_TYPE_UAV        : return "UAV"    ;
		case D3D12_DESCRIPTOR_RANGE_TYPE_CBV        : return "CBV"    ;
		case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER    : return "SAMPLER";
		default: return "";
	}
}

template<>
constexpr StrLit ax_enum_entry_strlit(const D3D12_RESOURCE_STATES& v) {
	switch (v) {
		case D3D12_RESOURCE_STATE_COMMON								: return "COMMON";
		case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER			: return "VERTEX_AND_CONSTANT_BUFFER";
		case D3D12_RESOURCE_STATE_INDEX_BUFFER							: return "INDEX_BUFFER";
		case D3D12_RESOURCE_STATE_RENDER_TARGET							: return "RENDER_TARGET";
		case D3D12_RESOURCE_STATE_UNORDERED_ACCESS						: return "UNORDERED_ACCESS";
		case D3D12_RESOURCE_STATE_DEPTH_WRITE							: return "DEPTH_WRITE";
		case D3D12_RESOURCE_STATE_DEPTH_READ							: return "DEPTH_READ";
		case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE				: return "NON_PIXEL_SHADER_RESOURCE";
		case D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE					: return "PIXEL_SHADER_RESOURCE";
		case D3D12_RESOURCE_STATE_STREAM_OUT							: return "STREAM_OUT";
		case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT						: return "INDIRECT_ARGUMENT";
		case D3D12_RESOURCE_STATE_COPY_DEST								: return "COPY_DEST";
		case D3D12_RESOURCE_STATE_COPY_SOURCE							: return "COPY_SOURCE";
		case D3D12_RESOURCE_STATE_RESOLVE_DEST							: return "RESOLVE_DEST";
		case D3D12_RESOURCE_STATE_RESOLVE_SOURCE						: return "RESOLVE_SOURCE";
		case D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE		: return "RAYTRACING_ACCELERATION_STRUCTURE";
		case D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE					: return "SHADING_RATE_SOURCE";
		case D3D12_RESOURCE_STATE_RESERVED_INTERNAL_8000				: return "RESERVED_INTERNAL_8000";
		case D3D12_RESOURCE_STATE_RESERVED_INTERNAL_4000				: return "RESERVED_INTERNAL_4000";
		case D3D12_RESOURCE_STATE_RESERVED_INTERNAL_100000				: return "RESERVED_INTERNAL_100000";
		case D3D12_RESOURCE_STATE_RESERVED_INTERNAL_40000000			: return "RESERVED_INTERNAL_40000000";
		case D3D12_RESOURCE_STATE_RESERVED_INTERNAL_80000000			: return "RESERVED_INTERNAL_80000000";
		case D3D12_RESOURCE_STATE_GENERIC_READ							: return "GENERIC_READ";
		case D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE					: return "ALL_SHADER_RESOURCE";
//		case D3D12_RESOURCE_STATE_PRESENT								: return "PRESENT";
//		case D3D12_RESOURCE_STATE_PREDICATION							: return "PREDICATION";
		case D3D12_RESOURCE_STATE_VIDEO_DECODE_READ						: return "VIDEO_DECODE_READ";
		case D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE					: return "VIDEO_DECODE_WRITE";
		case D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ					: return "VIDEO_PROCESS_READ";
		case D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE					: return "VIDEO_PROCESS_WRITE";
		case D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ						: return "VIDEO_ENCODE_READ";
		case D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE					: return "VIDEO_ENCODE_WRITE";
		default: return "";
	}
}

template <class FMT_CH>
class FormatHandler<D3D12_RESOURCE_STATES, FMT_CH> {
public:
	using Obj = D3D12_RESOURCE_STATES;
	void onFormat(const Obj & obj, Format_<FMT_CH> & fmt) {
		fmt << Fmt("({:08x}:{})", ax_enum_int(obj), ax_enum_str(obj));
	}
};


} // namespace

#endif // #if AX_RENDERER_DX12
