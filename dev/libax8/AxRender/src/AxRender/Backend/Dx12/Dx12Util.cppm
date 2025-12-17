module;

export module AxRender:Dx12Util;

#if AX_RENDERER_DX12
export import :Shader;
export import :Texture;
export import :CommandBuffer;

namespace ax /*::AxRender*/ {

class Renderer_Dx12;

using AX_DX12_IDXGIFactory    = IDXGIFactory4;
using AX_DX12_ID3D12Device    = ID3D12Device;
using AX_DX12_IDXGIAdapter    = IDXGIAdapter3;
using AX_DX12_IDXGISwapChain  = IDXGISwapChain3;

#if _DEBUG
	using AX_DX12_IDXGIDebug	= IDXGIDebug1;
	using AX_DX12_ID3D12Debug	= ID3D12Debug;
#endif

struct Dx12Util {
	Dx12Util() = delete;

	class DxResourceCreator;

	static constexpr bool isValid		(HRESULT hr) { if (!checkError(hr)) { reportError(hr); return false; } return true; }

	static constexpr void throwIfError	(HRESULT hr) { if (!checkError(hr)) { reportError(hr); throw Error_Undefined(); } }
	static constexpr void throwIfError	(HRESULT hr, ID3DBlob* error);

	static constexpr void assertIfError	(HRESULT hr) { if (!checkError(hr)) { reportError(hr); AX_ASSERT(false); } }
	static inline	 void reportError	(HRESULT hr);

	static constexpr bool checkError(HRESULT hr) {
		if (FAILED(hr))
			return false;
		return true;
	}
	
	static constexpr D3D12_PRIMITIVE_TOPOLOGY		getDxPrimitiveTopology		(RenderPrimitiveType t);
	static constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE	getDxPrimitiveTopologyType	(RenderPrimitiveType t);
	static constexpr D3D12_FILTER					getDxSamplerFilter			(SamplerFilter v);
	static constexpr D3D12_TEXTURE_ADDRESS_MODE		getDxSamplerWrap			(SamplerWrap v);
	static constexpr DXGI_FORMAT					getDxIndexType				(IndexType t);
	static constexpr D3D12_COMPARISON_FUNC			getDxDepthTestOp			(DepthTestOp v);

	static constexpr DXGI_FORMAT		getDxColorType		(ColorType type);
	static constexpr DXGI_FORMAT		getDxDepthType		(RenderDepthType type);
	static constexpr DXGI_FORMAT		getDxDataType		(RenderDataType v);
	static constexpr D3D12_BLEND_OP		getDxBlendOp		(BlendOp		v);
	static constexpr D3D12_BLEND		getDxBlendFactor	(BlendFactor	v);
	static constexpr D3D12_CULL_MODE	getDxCullMode		(CullMode		v);

	static constexpr D3D12_COMMAND_LIST_TYPE getDxCommandBufferType(CommandBufferType type);
	
	static constexpr StrView	errorToStrView(ID3DBlob* blob);

	static constexpr UINT		castUINT	(Int v) { return ax_safe_cast(v); }
	static constexpr UINT16	castUINT16	(Int v) { return ax_safe_cast(v); }
	static constexpr UINT64	castUINT64	(Int v) { return ax_safe_cast(v); }
};

inline void Dx12Util::reportError(HRESULT hr) {
	_com_error err(hr);
	auto errMsg = StrView_c_str(err.ErrorMessage());
	AX_LOG_ERROR("HRESULT = {:x} {}", static_cast<u32>(hr), errMsg); 
	
#if 0 && _DEBUG
	auto* d = renderer()->dxgiDebug();
	if (d) {
		d->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	}
#endif
	AX_ASSERT(false);
}

constexpr void Dx12Util::throwIfError(HRESULT hr, ID3DBlob* error) {
	if (!checkError(hr)) {
		if (error) {
			AX_LOG("DX12 error: {?}", errorToStrView(error));
		}
		throw Error_Undefined();
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

constexpr DXGI_FORMAT Dx12Util::getDxIndexType(IndexType t) {
	using SRC = IndexType;
	switch (t) {
		case SRC::UInt16:	return DXGI_FORMAT_R16_UINT;
		case SRC::UInt32:	return DXGI_FORMAT_R32_UINT;
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
		case SRC::u8:		return DXGI_FORMAT_R8_UNORM; break;
		case SRC::u8x2:		return DXGI_FORMAT_R8G8_UNORM; break;
//		case SRC::u8x3:		return DXGI_FORMAT_R8G8B8_UNORM; break; //does not support in DX11
		case SRC::u8x4:		return DXGI_FORMAT_R8G8B8A8_UNORM; break;
	//--
		case SRC::f16:		return DXGI_FORMAT_R16_FLOAT; break;
		case SRC::f16x2:	return DXGI_FORMAT_R16G16_FLOAT; break;
//		case SRC::f16x3:	return DXGI_FORMAT_R16G16B16_FLOAT; break; //does not support in DX11
		case SRC::f16x4:	return DXGI_FORMAT_R16G16B16A16_FLOAT; break;
	//---
		case SRC::f32:		return DXGI_FORMAT_R32_FLOAT; break;
		case SRC::f32x2:	return DXGI_FORMAT_R32G32_FLOAT; break;
		case SRC::f32x3:	return DXGI_FORMAT_R32G32B32_FLOAT; break;
		case SRC::f32x4:	return DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	//---
		default: throw Error_Undefined();
	}
}

constexpr D3D12_BLEND_OP Dx12Util::getDxBlendOp(BlendOp v) {
	using SRC = BlendOp;
	switch (v) {
		case SRC::Add:		return D3D12_BLEND_OP_ADD;
		case SRC::Sub:		return D3D12_BLEND_OP_SUBTRACT;
		case SRC::RevSub:	return D3D12_BLEND_OP_REV_SUBTRACT;
		case SRC::Min:		return D3D12_BLEND_OP_MIN;
		case SRC::Max:		return D3D12_BLEND_OP_MAX;
		default: throw Error_Undefined();
	}
}

constexpr D3D12_BLEND Dx12Util::getDxBlendFactor(BlendFactor v) {
	using SRC = BlendFactor;
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

constexpr D3D12_CULL_MODE Dx12Util::getDxCullMode(CullMode v) {
	using SRC = CullMode;
	switch (v) {
		case SRC::None:	return D3D12_CULL_MODE_NONE;
		case SRC::Back:	return D3D12_CULL_MODE_BACK;
		case SRC::Front:	return D3D12_CULL_MODE_FRONT;
		default: throw Error_Undefined();
	}
}

constexpr D3D12_COMMAND_LIST_TYPE Dx12Util::getDxCommandBufferType(CommandBufferType type) {
	using SRC = CommandBufferType;
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

constexpr D3D12_COMPARISON_FUNC Dx12Util::getDxDepthTestOp(DepthTestOp v) {
	using SRC = DepthTestOp;
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

} // namespace

#endif // #if AX_RENDERER_DX12
