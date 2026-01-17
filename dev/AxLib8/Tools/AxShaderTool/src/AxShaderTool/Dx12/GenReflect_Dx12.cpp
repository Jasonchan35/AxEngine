module AxShaderTool;

#if AX_RENDERER_DX12

import :GenReflect_Dx12;

namespace ax /*::AxRender*/ {

class GenReflect_Dx12::IncludeHandler : public IDxcIncludeHandler {
public:
	virtual ~IncludeHandler() = default;

	IncludeHandler(GenReflect_Dx12* compiler, StrView currentFile) 
		: _compiler(compiler)
	{
		_currentFileDir = FilePath::dirname(currentFile);
	}

	virtual HRESULT STDMETHODCALLTYPE LoadSource(
		_In_ LPCWSTR pFilename,                                   // Candidate filename.
		_COM_Outptr_result_maybenull_ IDxcBlob **ppIncludeSource  // Resultant source object for included file, nullptr if not found.
	) override
	{
		auto filename = String::s_utf(StrView_c_str(pFilename));

		TempString fullpath;
		if (FilePath::isAbsPath(filename)) {
			fullpath = filename;
		} else {
			fullpath = Fmt("{}/{}", _currentFileDir, filename);
		}

		ComPtr<IDxcBlobEncoding> fileBlob;

		if (!_compiler->tryLoadFile(fileBlob, fullpath)) {
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}

		*ppIncludeSource = fileBlob.detach();

		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef	() override { return 10; }
	virtual ULONG STDMETHODCALLTYPE Release	() override { return 10; }
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject) override { return E_FAIL; }

private:
	String			_currentFileDir;
	GenReflect_Dx12*		_compiler;
};

GenReflect_Dx12::GenReflect_Dx12() {
	HRESULT hr;

AX_GCC_WARNING_PUSH_AND_DISABLE("-Wlanguage-extension-token")
	hr = DxcCreateInstance(CLSID_DxcLibrary,  IID_PPV_ARGS(_dxcLib.ptrForInit()));
	throwIfError(hr);

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(_dxc.ptrForInit()));
	throwIfError(hr);
AX_GCC_WARNING_POP()

}

bool GenReflect_Dx12::tryLoadFile(ComPtr<IDxcBlobEncoding>& outBlob, StrView filename) {
	outBlob.unref();

	if (!File::exists(filename))
		return false;

	TempStringW filenameW;
	filenameW.setUtf(filename);

	UINT32 codePage = 0;
	HRESULT hr;
	hr = _dxcLib->CreateBlobFromFile(filenameW.c_str(), &codePage, outBlob.ptrForInit());
	if (!_checkError(hr)) {
		return false;
	}

	if (!outBlob) {
		return false;
	}

	auto& f = _openedFileBlob.emplaceBack();
	f.filename = filename;
	f.blob.ref(outBlob);
	return true;
}

void GenReflect_Dx12::writeDepFile(StrView filename) {
	String outFilename = filename;
	outFilename.append(".d");

	String o = filename;
	o.append(": \\\n");

	for (auto& f : _openedFileBlob) {
		o.append("\t");
		o.append(FilePath::nativePath(f.filename));
		o.append(" \\\n");
	}
	o.append("\n");

	auto& opt = CmdOptions::s_instance();
	File::writeFile(outFilename, o, opt.writeFileOpt);
}

void GenReflect_Dx12::compile(StrView      outFilename,
                              StrView      filename,
                              StrView      profile,
                              StrView      entryFunc,
                              Span<String> include_dirs,
                              bool         keepUnusedVariable
) {
	_keepUnusedVariable = keepUnusedVariable;

	String cur;
	FilePath::getCurrentDir(cur);
	//AX_LOG("Current dir = {}", cur);
	//AX_DUMP_VAR(entryFunc, profile, include_dirs);

	auto filenameW	= TempStringW::s_utf(filename);
	auto entryFuncW	= TempStringW::s_utf(entryFunc);
	auto profileW	= TempStringW::s_utf(profile);

	HRESULT hr;

	ComPtr<IDxcBlobEncoding> sourceBlob;
	if (!tryLoadFile(sourceBlob, filename)) {
		AX_LOG("File not found {}", filename);
		return;
	}

	struct Args {
		void add(StrViewW s) { _args.emplaceBack().setUtf(s); }
		void add(StrView  s) { _args.emplaceBack().setUtf(s); }

		MutSpan<LPCWSTR>	output() {
			_output.clear();
			for (auto& e : _args) {
				_output.emplaceBack(e.c_str());
			}
			return _output;
		}
	private:
		Array<StringW,	64>	_args;
		Array<LPCWSTR,	64>	_output;
	};

	struct Defines {
		void add(StrViewW name, StrViewW value) {
			 _name.emplaceBack().setUtf(name);
			_value.emplaceBack().setUtf(value);
		}

		void add(StrView name, StrView value) {
			 _name.emplaceBack().setUtf(name);
			_value.emplaceBack().setUtf(value);
		}

		MutSpan<DxcDefine>	output() {
			_output.clear();
			AX_ASSERT(_name.size() == _value.size());
			Int n = _name.size();
			for (Int i = 0; i < n; i++) {
				auto& v = _value[i];

				DxcDefine	e;
				e.Name  = _name[i].c_str();
				e.Value = v ? v.c_str() : nullptr;

				_output.emplaceBack(e);
			}

			return _output;
		}

	private:
		Array<StringW,	64>	_name;
		Array<StringW,  64>	_value;
		Array<DxcDefine,64>	_output;
	};

	Args args;

	args.add("-WX"); // treat warning as error

	for (auto& d : include_dirs) {
		args.add(Fmt("-I{}", d));
	}

	Defines defines;
#if AX_RENDER_BINDLESS
	defines.add("AX_RENDER_BINDLESS", "1");
#endif

	IncludeHandler includeHandler(this, filename);
	ComPtr<IDxcOperationResult>	result;

	HRESULT resultStatus;

	auto outArgs	= args.output();
	auto outDefines	= defines.output();

	if (0) {
		hr = _dxc->Preprocess(	sourceBlob, filenameW.c_str(), 
								outArgs.data(),    ax_safe_cast_from(outArgs.size()),
								outDefines.data(), ax_safe_cast_from(outDefines.size()),
								&includeHandler, result.ptrForInit());
		throwIfError(hr);
		hr = result->GetStatus(&resultStatus);
		throwIfError(hr);

		ComPtr<IDxcBlob> resultDetail;
		hr = result->GetResult(resultDetail.ptrForInit());
		throwIfError(hr);

		auto data = StrView_make(resultDetail);
		AX_LOG("preprocess: ============= \n{}\n===============\n", data);
	}

	ComPtr<IDxcBlob>	debugBlob;
	hr = _dxc->CompileWithDebug(	sourceBlob,	filenameW.c_str(),
									entryFuncW.c_str(),
									profileW.c_str(),
									outArgs.data(),		ax_safe_cast_from(outArgs.size()),
									outDefines.data(),	ax_safe_cast_from(outDefines.size()),
									&includeHandler, result.ptrForInit(),
									nullptr, debugBlob.ptrForInit());
	throwIfError(hr);

//	HRESULT resultStatus;
	hr = result->GetStatus(&resultStatus);
	throwIfError(hr);

	if (!_checkError(resultStatus)) {
		ComPtr<IDxcBlobEncoding> errorMsgBlob;
		hr = result->GetErrorBuffer(errorMsgBlob.ptrForInit());
		throwIfError(hr);

		AX_WinBOOL known;
		UINT32 codePage = CP_UTF8;
		hr = errorMsgBlob->GetEncoding(&known, &codePage);
		throwIfError(hr);

		if (codePage != CP_UTF8) {
			AX_LOG("Error message code page is not UTF8");
		}

		auto errorMsg = StrView_make(errorMsgBlob);
		AX_LOG("error: {}", errorMsg);
		return;
	}

	auto debugMsg = StrView_make(debugBlob);
	if (debugMsg) {
		AX_LOG("debug message: {}", debugMsg);
	}

	ComPtr<IDxcBlob>	byteCode;
	hr = result->GetResult(byteCode.ptrForInit());
	throwIfError(hr);

	writeDepFile(outFilename);

	// have to use dxc.exe from MS, because that add hash in the bin file
	//ByteSpan byteCodeView(reinterpret_cast<const Byte*>(byteCode->GetBufferPointer()),
	// 							 SafeCast(byteCode->GetBufferSize()));

	//auto u = File::writeFile(outFilename, byteCodeView, false, false);
	//if (u == '=') {
	//	axFile::touch(outFilename);
	//}

	_compileReflect(outFilename, byteCode, profile);
}

void GenReflect_Dx12::_compileReflect(StrView outFilename, IDxcBlob* byteCode, StrView profile) {

	ComPtr<IDxcContainerReflection>	container;
	ComPtr<ID3D12ShaderReflection>	reflect;

	HRESULT hr;

AX_GCC_WARNING_PUSH_AND_DISABLE("-Wlanguage-extension-token")
	hr = DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(container.ptrForInit()));
	throwIfError(hr);
AX_GCC_WARNING_POP()

	hr = container->Load(byteCode);
	throwIfError(hr);

	UINT shaderIdx;
	hr = container->FindFirstPartKind(CharUtil::FourCC("DXIL"), &shaderIdx);
	throwIfError(hr);

AX_GCC_WARNING_PUSH_AND_DISABLE("-Wlanguage-extension-token")
	hr = container->GetPartReflection(shaderIdx, IID_PPV_ARGS(reflect.ptrForInit()));
AX_GCC_WARNING_POP()

	if (_checkError(hr)) {
		D3D12_SHADER_DESC desc;
		hr = reflect->GetDesc(&desc);
		throwIfError(hr);

		ShaderStageInfo	outInfo;

		auto programType = (desc.Version & 0xFFFF0000) >> 16;
		switch (programType) {
			case D3D12_SHVER_VERTEX_SHADER:		outInfo.stageFlags = ShaderStageFlags::Vertex;		break;
			case D3D12_SHVER_PIXEL_SHADER:		outInfo.stageFlags = ShaderStageFlags::Pixel;		break;
			case D3D12_SHVER_GEOMETRY_SHADER:	outInfo.stageFlags = ShaderStageFlags::Geometry;	break;
			case D3D12_SHVER_COMPUTE_SHADER:	outInfo.stageFlags = ShaderStageFlags::Compute;		break;
			default: {
				AX_ASSERT(false);
				AX_LOG("Unknown shader stage type {}", programType);
			} break;
		}

		{
			_compileReflect_inputs				(outInfo, reflect, desc);
			_compileReflect_constBuffers		(outInfo, reflect, desc);
			_compileReflect_textures			(outInfo, reflect, desc);
			_compileReflect_samplers			(outInfo, reflect, desc);
			_compileReflect_structuredBuffers	(outInfo, reflect, desc);
		}

		{
			String jsonFilename = outFilename;
			// jsonFilename.append(".json");
			auto& opt = CmdOptions::s_instance();
			JsonIO::writeFile(jsonFilename, outInfo, opt.writeFileOpt);
		}
	}
}

void GenReflect_Dx12::_compileReflect_inputs(ShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc) {
	HRESULT hr;

	outInfo.inputs.ensureCapacity(desc.InputParameters);

	for (UINT i=0; i<desc.InputParameters; i++) {
		auto& dst = outInfo.inputs.emplaceBack();

		D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
		hr = reflect->GetInputParameterDesc(i, &paramDesc);
		throwIfError(hr);

		TempString semantic = StrView_c_str(paramDesc.SemanticName);

		if (semantic == "TEXCOORD"
		 || semantic == "COLOR"
		 || semantic == "NORMAL"
		 || semantic == "TANGENT"
		 || semantic == "BINORMAL"
		) {
			FmtTo(semantic, "{}", paramDesc.SemanticIndex);
		}

		if (!semantic.tryParse(dst.semantic)) {
			throw Error_Undefined(Fmt("Unknown semantic name {}", semantic));
		}

		TempString dataType;

		auto componentCount = ax_bit_count1(paramDesc.Mask);
		if (componentCount < 1 || componentCount > 4) {
			throw Error_Undefined();
		}

		if (componentCount > 1) {
			dataType.append(Fmt("Vec{}", componentCount));
			switch (paramDesc.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32:		dataType.append("u32"); break;
				case D3D_REGISTER_COMPONENT_SINT32:		dataType.append("i32");	break;
				case D3D_REGISTER_COMPONENT_FLOAT16:	dataType.append("h");   break;
				case D3D_REGISTER_COMPONENT_FLOAT32:	dataType.append("f");   break;
				case D3D_REGISTER_COMPONENT_FLOAT64:	dataType.append("d");   break;
				default: throw Error_Undefined();
			}
		} else {
			switch (paramDesc.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32:		dataType.append("u32"); break;
				case D3D_REGISTER_COMPONENT_SINT32:		dataType.append("i32");	break;
				case D3D_REGISTER_COMPONENT_FLOAT16:	dataType.append("f16"); break;
				case D3D_REGISTER_COMPONENT_FLOAT32:	dataType.append("f32"); break;
				case D3D_REGISTER_COMPONENT_FLOAT64:	dataType.append("f64"); break;
				default: throw Error_Undefined();
			}
		}
		
		if (!dataType.tryParse(dst.dataType)) {
			AX_LOG("Error: parse enum {}", dataType);
			throw Error_Undefined();
		}

		//wr.member("register", paramDesc.Register);
	}
}

RenderDataType GenReflect_Dx12::_getRenderDataType(const D3D12_SHADER_TYPE_DESC& t) {
	switch (t.Class) {
		case D3D_SVC_SCALAR: {
			switch (t.Type) {
				case D3D_SVT_BOOL:				return RenderDataType::Bool;
				case D3D_SVT_INT:				return RenderDataType::i32;
				case D3D_SVT_UINT:				return RenderDataType::u32;
				case D3D_SVT_UINT8:				return RenderDataType::u8;
				case D3D_SVT_FLOAT:				return RenderDataType::f32;
				case D3D_SVT_DOUBLE:			return RenderDataType::f64;
				default: throw Error_Undefined();
			}
		} break;
			
		case D3D_SVC_VECTOR: {
			switch (t.Type) {
				case D3D_SVT_INT: {
					switch (t.Columns) {
						case 1: return RenderDataType::Vec1i32;
						case 2: return RenderDataType::Vec2i32;
						case 3: return RenderDataType::Vec3i32;
						case 4: return RenderDataType::Vec4i32;
						default: throw Error_Undefined();
					}
				} break;
				case D3D_SVT_UINT: {
					switch (t.Columns) {
						case 1: return RenderDataType::Vec1u32;
						case 2: return RenderDataType::Vec2u32;
						case 3: return RenderDataType::Vec3u32;
						case 4: return RenderDataType::Vec4u32;
						default: throw Error_Undefined();
					}
				} break;
				case D3D_SVT_UINT8: {
					switch (t.Columns) {
						case 1: return RenderDataType::Vec1u8;
						case 2: return RenderDataType::Vec2u8;
						case 3: return RenderDataType::Vec3u8;
						case 4: return RenderDataType::Vec4u8;
						default: throw Error_Undefined();
					}
				} break;
				case D3D_SVT_FLOAT: {
					switch (t.Columns) {
						case 1: return RenderDataType::Vec1f;
						case 2: return RenderDataType::Vec2f;
						case 3: return RenderDataType::Vec3f;
						case 4: return RenderDataType::Vec4f;
						default: throw Error_Undefined();
					}
				} break;
				case D3D_SVT_DOUBLE: {
					switch (t.Columns) {
						case 1: return RenderDataType::Vec1d;
						case 2: return RenderDataType::Vec2d;
						case 3: return RenderDataType::Vec3d;
						case 4: return RenderDataType::Vec4d;
						default: throw Error_Undefined();
					}
				} break;
				default: throw Error_Undefined();
			}
		} break;

		case D3D_SVC_MATRIX_ROWS:
		case D3D_SVC_MATRIX_COLUMNS:	{
			if (t.Rows == 4 && t.Columns == 4) {
				switch (t.Type) {
					case D3D_SVT_FLOAT:  return RenderDataType::Mat4f;
					case D3D_SVT_DOUBLE: return RenderDataType::Mat4d;
					default: throw Error_Undefined();
				}
			}
		} break;
		default: throw Error_Undefined();
	}
	
	throw Error_Undefined();
}

void GenReflect_Dx12::_compileReflect_constBuffers(ShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc) {
	HRESULT hr;

	outInfo.constBuffers.ensureCapacity(desc.BoundResources);

	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D12_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		throwIfError(hr);

		if (resDesc.Type != D3D_SIT_CBUFFER) continue;

		auto& outCB      = outInfo.constBuffers.emplaceBack();
		outCB.stageFlags = outInfo.stageFlags;
		outCB.dataType   = RenderDataType::ConstBuffer;

		D3D12_SHADER_BUFFER_DESC bufDesc;
		auto* cb = reflect->GetConstantBufferByName(resDesc.Name);
		hr = cb->GetDesc(&bufDesc);
		throwIfError(hr);

		outCB.name       = StrView_c_str(bufDesc.Name);
		outCB.bindPoint  = ax_safe_cast_from(resDesc.BindPoint);
		outCB.bindCount  = ax_safe_cast_from(resDesc.BindCount);
		outCB.bindSpace  = ax_safe_cast_from(resDesc.Space);
		outCB.bufferSize = ax_safe_cast_from(bufDesc.Size);

		outCB.variables.ensureCapacity(bufDesc.Variables);
		for (UINT j=0; j<bufDesc.Variables; j++) {
			auto* cbv = cb->GetVariableByIndex(j);
			D3D12_SHADER_VARIABLE_DESC varDesc;
			hr = cbv->GetDesc(&varDesc);
			throwIfError(hr);

			D3D12_SHADER_TYPE_DESC varType;
			hr = cbv->GetType()->GetDesc(&varType);
			throwIfError(hr);

			if (!_keepUnusedVariable) {
				if (0 == (varDesc.uFlags & D3D_SVF_USED)) continue;
			}

			auto& outVar  = outCB.variables.emplaceBack();
			outVar.name   = StrView_c_str(varDesc.Name);
			outVar.offset = ax_safe_cast_from(varDesc.StartOffset);
					
			outVar.dataType = _getRenderDataType(varType);

			if (outVar.dataType == RenderDataType::None) {
				throw Error_Undefined();
			}
		}
	}
}

void GenReflect_Dx12::_compileReflect_structuredBuffers(ShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc) {
	HRESULT hr;
	outInfo.structuredBuffers.ensureCapacity(desc.BoundResources);
	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D12_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		throwIfError(hr);

		if (resDesc.Type != D3D_SIT_STRUCTURED) continue;
		
		D3D12_SHADER_BUFFER_DESC bufDesc;
		auto* cb = reflect->GetConstantBufferByName(resDesc.Name);
		hr = cb->GetDesc(&bufDesc);
		throwIfError(hr);
		
		auto& outBufferInfo      = outInfo.structuredBuffers.emplaceBack();
		outBufferInfo.stageFlags = outInfo.stageFlags;
		outBufferInfo.dataType   = RenderDataType::StructuredBuffer;
		outBufferInfo.name       = StrView_c_str(resDesc.Name);
		outBufferInfo.bindPoint  = ax_safe_cast_from(resDesc.BindPoint);
		outBufferInfo.bindCount  = ax_safe_cast_from(resDesc.BindCount);
		outBufferInfo.bindSpace  = ax_safe_cast_from(resDesc.Space);
		outBufferInfo.bufferSize = ax_safe_cast_from(bufDesc.Size);
		
		if (bufDesc.Variables < 1) throwIfError(hr);
		auto* cbv = cb->GetVariableByIndex(0);

		auto* varType = cbv->GetType();
		D3D12_SHADER_TYPE_DESC varTypeDesc;
		hr = varType->GetDesc(&varTypeDesc);
		throwIfError(hr);

		for (UINT j=0; j<varTypeDesc.Members; j++) {
			auto* memberName = varType->GetMemberTypeName(j);
			auto* memberType = varType->GetMemberTypeByIndex(j);
			D3D12_SHADER_TYPE_DESC memberTypeDesc;
			hr = memberType->GetDesc(&memberTypeDesc);
			throwIfError(hr);
			
			auto& outVar  = outBufferInfo.variables.emplaceBack();
			outVar.name   = StrView_c_str(memberName);
			outVar.offset = ax_safe_cast_from(memberTypeDesc.Offset);

			outVar.dataType = _getRenderDataType(memberTypeDesc);
			if (outVar.dataType == RenderDataType::None) {
				throw Error_Undefined();
			}
		}		
	}
}

void GenReflect_Dx12::_compileReflect_textures(ShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc) {
	HRESULT hr;

	outInfo.textures.ensureCapacity(desc.BoundResources);
	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D12_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		throwIfError(hr);

		if (resDesc.Type != D3D_SIT_TEXTURE) continue;

		auto& outTex      = outInfo.textures.emplaceBack();
		outTex.stageFlags = outInfo.stageFlags;
		outTex.name       = StrView_c_str(resDesc.Name);
		outTex.bindPoint  = ax_safe_cast_from(resDesc.BindPoint);
		outTex.bindCount  = ax_safe_cast_from(resDesc.BindCount);
		outTex.bindSpace  = ax_safe_cast_from(resDesc.Space);

		switch (resDesc.Dimension) {
			case D3D_SRV_DIMENSION_TEXTURE1D:		outTex.dataType = RenderDataType::Texture1D;   break;
			case D3D_SRV_DIMENSION_TEXTURE2D:		outTex.dataType = RenderDataType::Texture2D;   break;
			case D3D_SRV_DIMENSION_TEXTURE3D:		outTex.dataType = RenderDataType::Texture3D;   break;
			case D3D_SRV_DIMENSION_TEXTURECUBE:		outTex.dataType = RenderDataType::TextureCube; break;
		//----
			case D3D_SRV_DIMENSION_TEXTURE1DARRAY:	outTex.dataType = RenderDataType::Texture1DArray;   break;
			case D3D_SRV_DIMENSION_TEXTURE2DARRAY:	outTex.dataType = RenderDataType::Texture2DArray;   break;
			case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:outTex.dataType = RenderDataType::TextureCubeArray; break;
		//----
			default: throw Error_Undefined();
		}

		//switch (resDesc.ReturnType) {
		//	case D3D_RETURN_TYPE_UNORM:		wr.member("returnType", "UNORM");	 break;
		//	case D3D_RETURN_TYPE_SNORM:		wr.member("returnType", "SNORM");	 break;
		//	case D3D_RETURN_TYPE_SINT:		wr.member("returnType", "SINT");	 break;
		//	case D3D_RETURN_TYPE_UINT:		wr.member("returnType", "UINT");	 break;
		//	case D3D_RETURN_TYPE_FLOAT:		wr.member("returnType", "FLOAT");	 break;
		//	case D3D_RETURN_TYPE_DOUBLE:	wr.member("returnType", "DOUBLE");	 break;
		//	default: throw Error_Undefined();
		//}
	}
}

void GenReflect_Dx12::_compileReflect_samplers(ShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc) {
	HRESULT hr;
	outInfo.samplers.ensureCapacity(desc.BoundResources);
	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D12_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		throwIfError(hr);

		if (resDesc.Type != D3D_SIT_SAMPLER) continue;

		auto& outSampler      = outInfo.samplers.emplaceBack();
		outSampler.stageFlags = outInfo.stageFlags;
		outSampler.dataType   = RenderDataType::SamplerState;
		outSampler.name       = StrView_c_str(resDesc.Name);
		outSampler.bindPoint  = ax_safe_cast_from(resDesc.BindPoint);
		outSampler.bindCount  = ax_safe_cast_from(resDesc.BindCount);
		outSampler.bindSpace  = ax_safe_cast_from(resDesc.Space);
	}
}

} //namespace

#endif // #if AX_RENDERER_DX12

