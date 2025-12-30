module;
#if !AX_RENDERER_DX12
export module AxShaderTool:GenReflect_Dx12;
#else
	#if AX_OS_WINDOWS
		using AX_WinBOOL = BOOL;
		#include <dxcapi.h> // from Windows SDK
	//	#include <dxc/dxcapi.h> // from Vulkan SDK
		#include <d3d12shader.h>
		#pragma comment(lib, "dxcompiler.lib")
	#else
		#define BOOL axWinBOOL
		using AX_WinBOOL = bool;
		#include <dxc/dxcapi.h>
		AX_STATIC_ASSERT(std::is_same_v<BOOL, AX_WinBOOL>);
		#undef BOOL
	#endif

export module AxShaderTool:GenReflect_Dx12;
export import :GenNinja_Dx12;

export namespace ax /*::AxRender*/ {

inline
StrView StrView_make(IDxcBlob* blob) {
	if (blob == nullptr)
		return StrView();
	auto* s = static_cast<const char*>(blob->GetBufferPointer());
	auto  n = static_cast<Int>(blob->GetBufferSize());
	return StrView(s, n);
}

class GenReflect_Dx12 : public NonCopyable {
public:
	GenReflect_Dx12();

	void compile(StrView	  outFilename,
				 StrView	  filename,
				 StrView	  profile,
				 StrView	  entryFunc,
				 Span<String> include_dirs,
				 bool		  keepUnusedVariable);

	bool tryLoadFile(ComPtr<IDxcBlobEncoding>& outBlob, StrView filename);

	void writeDepFile(StrView filename);

private:
	void _compileReflect(StrView outFilename, IDxcBlob* byteCode, StrView profile);
	void _compileReflect_inputs			(ShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc);
	void _compileReflect_constBuffers	(ShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc);
	void _compileReflect_textures		(ShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc);
	void _compileReflect_samplers		(ShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc);
	void _compileReflect_storageBuffers	(ShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc);

	class IncludeHandler;
	
	bool checkError(HRESULT hr) {
		if (FAILED(hr))
			return false;
		return true;
	}

	void throwIfError(HRESULT hr) {
		if (!checkError(hr)) {
			throw Error_Undefined();
		}
	}

	ComPtr<IDxcLibrary>				_dxcLib;
	ComPtr<IDxcCompiler2>				_dxc;

	struct OpenedFile {
		String filename;
		ComPtr<IDxcBlobEncoding>	blob;
	};
	Array<OpenedFile>		_openedFileBlob;

	bool _keepUnusedVariable = false;
};

} // namespace 

#endif // #if AX_RENDERER_DX12