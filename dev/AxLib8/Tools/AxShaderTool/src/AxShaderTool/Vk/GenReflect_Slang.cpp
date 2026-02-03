module;

#if AX_RENDERER_VK

module AxShaderTool;
import :GenReflect_Slang;

namespace ax /*::AxRender*/ {

ShaderStageFlags GenReflect_Slang::_getShaderStageFlags(StrView s) {
	if (s == "vertex"       ) return ShaderStageFlags::Vertex;
	if (s == "fragment"     ) return ShaderStageFlags::Pixel;
	if (s == "geometry"     ) return ShaderStageFlags::Geometry;
	if (s == "compute"      ) return ShaderStageFlags::Compute;
	if (s == "mesh"         ) return ShaderStageFlags::Mesh;
	if (s == "amplification") return ShaderStageFlags::Amplification;
	return ShaderStageFlags::None;
}

VertexSemantic	GenReflect_Slang::_getShaderSemantic(StrView src) {
	VertexSemantic o;
	
	if (src == "COLOR"   ) return VertexSemantic::COLOR0;
	if (src == "TEXCOORD") return VertexSemantic::TEXCOORD0;
	if (src == "NORMAL"  ) return VertexSemantic::NORMAL0;
	if (src == "TANGENT" ) return VertexSemantic::TANGENT0;
	if (src == "BINORMAL") return VertexSemantic::BINORMAL0;
	
	if (!src.tryParse(o)) {
		throw Error_Runtime(Fmt("unknown semantic [{}]", src));
	}
	return o;
}

RenderDataType GenReflect_Slang::_getRenderDataType(const JsonObject& json_type) {
	auto& json_kind = json_type.memberString("kind");
	
	if (json_kind == "None") return RenderDataType::None;
	if (json_kind == "struct") return RenderDataType::Struct;
	
	if (json_kind == "scalar") {
		auto& scalarType = json_type.memberString("scalarType");
		if (scalarType == "uint8" ) return RenderDataType::u8;
		if (scalarType == "uint16") return RenderDataType::u16;
		if (scalarType == "uint32") return RenderDataType::u32;
		if (scalarType == "uint64") return RenderDataType::u64;
		
		if (scalarType == "int8"  ) return RenderDataType::i8;
		if (scalarType == "int16" ) return RenderDataType::i16;
		if (scalarType == "int32" ) return RenderDataType::i32;
		if (scalarType == "int64" ) return RenderDataType::i64;

		if (scalarType == "float16" ) return RenderDataType::f16;
		if (scalarType == "float32" ) return RenderDataType::f32;
		if (scalarType == "float64" ) return RenderDataType::f64;

		throw Error_Undefined(Fmt("unsupported scalarType {}", scalarType));
	}
	
	if (json_kind == "vector") {
		auto  elementCount = json_type.member("elementCount")->asInt64();
		auto& elementType  = json_type.memberObject("elementType");
		auto& scalarType   = elementType.memberString("scalarType");
		
		TempString tmp;
		     if (scalarType == "uint8" ) tmp << "u8";
		else if (scalarType == "uint16") tmp << "u16";
		else if (scalarType == "uint32") tmp << "u32";
		else if (scalarType == "uint64") tmp << "u64";

		else if (scalarType == "int8" ) tmp << "i8";
		else if (scalarType == "int16") tmp << "i16";
		else if (scalarType == "int32") tmp << "i32";
		else if (scalarType == "int64") tmp << "i64";
		
		else if (scalarType == "float16") tmp << "f16";
		else if (scalarType == "float32") tmp << "f32";
		else if (scalarType == "float64") tmp << "f64";
		else {
			throw Error_Undefined(Fmt("unsupported Vector scalarType {}", scalarType));
		}

		tmp << Fmt("x{}", elementCount);
		
		RenderDataType dataType;
		if (!tmp.tryParse(dataType)) {
			throw Error_Undefined(Fmt("unsupported Vector type {}", tmp));
		}
		return dataType;
	}
	
	if (json_kind == "matrix") {
		TempString tmp = "Mat";
		
		auto rowCount    = json_type.member("rowCount")->asInt64();
		auto columnCount = json_type.member("columnCount")->asInt64();
		
		if (rowCount == columnCount) {
			tmp << Fmt("{}", rowCount);
		} else {
			tmp << Fmt("{}x{}", rowCount, columnCount);
		}

		auto& json_elementType = json_type.memberObject("elementType");
		auto& scalarType       = json_elementType.memberString("scalarType");
		
		     if (scalarType == "float32") { tmp << "f"; }
		else if (scalarType == "float64") { tmp << "d"; }
		else {
			throw Error_Undefined(Fmt("unsupported matrix scalar type {}", scalarType));
		}
		
		RenderDataType dataType;
		if (!tmp.tryParse(dataType)) {
			throw Error_Undefined(Fmt("unsupported Vector type {}", tmp));
		}
		return dataType;
	}

	throw Error_Undefined(Fmt("unsupported type kind {}", json_kind));
	return RenderDataType::None;
}

ShaderVariableType GenReflect_Slang::_getShaderVariableType(const JsonObject& json_type) {
	ShaderVariableType o;
	auto& json_kind = json_type.memberString("kind");
		
	if (json_kind == "array") {
		o.elementCount = ax_safe_cast_from(json_type.member("elementCount")->asInt64());
		o.dataType     = _getRenderDataType(json_type.memberObject("elementType"));
		return o;
	} else {
		o.dataType     = _getRenderDataType(json_type);
	}
	return o;
}

void GenReflect_Slang::generate(StrView outFilename, StrView filename, RenderAPI api) {
	JsonValue json_root;
	JsonIO::readFile(filename, json_root);
	
	auto& json_entryPoints = json_root.member("entryPoints")->asArray();
	if (json_entryPoints.size() < 1) throw Error_Undefined("entryPoints size < 1");
	auto& json_entryPoint = json_entryPoints[0];
	
	ShaderStageInfo outInfo;
	outInfo.stageFlags = _getShaderStageFlags(json_entryPoint.member("stage")->asString());

	_globalConstBuffer.name = "$Globals";
	_globalConstBuffer.dataType = RenderDataType::ConstBuffer;
	_globalConstBuffer.stageFlags = outInfo.stageFlags;
	_globalConstBuffer.bindSpace = BindSpace::Default;
	_globalConstBuffer.bindPoint = BindPoint::Zero;
	_globalConstBuffer.bindCount = 1;
	
	_genVertexInputs	(outInfo, json_entryPoint);
	_genBindings		(outInfo, json_root);
	
	if (_globalConstBuffer.variables) {
		_globalConstBuffer.stride = 0; 
		for (auto& v : _globalConstBuffer.variables) {
			Math::max_itself(_globalConstBuffer.stride, v.offset + v.sizeInBytes); 
		} 
		outInfo.constBuffers.emplaceBack(_globalConstBuffer);
	}

	auto& opt = CmdOptions::s_instance();
	JsonIO::writeFile(outFilename, outInfo, opt.writeFileOpt);
}

void GenReflect_Slang::_genVertexInputs(ShaderStageInfo& outInfo, const JsonValue& json) {
	auto& parameters = json.member("parameters")->asArray();
	if (parameters.size() < 1) throw Error_Undefined("parameters size < 1");
	auto& parameter = parameters[0].asObject();
	auto& json_type = parameter.member("type")->asObject();
	auto& json_kind = json_type.member("kind")->asString();

	const JsonArray* json_pFields = &parameters;
	
	if (json_kind == "struct") {
		json_pFields = &json_type.member("fields")->asArray();
	}
	
	for (auto& json_field : *json_pFields) {
		auto& dst = outInfo.inputs.emplaceBack();
		if (auto* json_semanticName = json_field.findMember("semanticName")) {
			dst.semantic = _getShaderSemantic(json_semanticName->asString());
		}
		dst.dataType = _getRenderDataType(json_field.member("type")->asObject());
	}
}

void GenReflect_Slang::_genBindings(ShaderStageInfo& outInfo, const JsonValue& json_root) {
	auto& json_parameters = json_root.member("parameters")->asArray();

	for (auto& json_parameter_ : json_parameters) {
		auto& json_parameter = json_parameter_.asObject();

		SrcParam srcParam;
		srcParam.parameter = &json_parameter;
		
		srcParam.type = &json_parameter.memberObject("type");
		auto& json_kind = srcParam.type->memberString("kind");

		auto* json_elementType = srcParam.type; 
		auto* json_elementKind = &json_kind;
		
		if (json_kind == "array") {
			srcParam.elementType = &srcParam.type->memberObject("elementType");
			json_elementType = srcParam.elementType;
			json_elementKind = &srcParam.elementType->memberString("kind");
		}

		srcParam.binding = json_parameter.tryGetMemberObject("binding");
		if (!srcParam.binding) continue;
		
		auto& json_binding_kind = srcParam.binding->memberString("kind");
		if (json_binding_kind == "uniform"       ) { _genGlobalParam(outInfo, srcParam); continue; }
		if (*json_elementKind == "constantBuffer") { _genConstBuffer(outInfo, srcParam); continue; }
		if (*json_elementKind == "samplerState"  ) { _genSampler    (outInfo, srcParam); continue; }
		if (*json_elementKind == "resource") {
			auto& json_baseShape = json_elementType->memberString("baseShape");
			if (json_baseShape == "texture2D") {
				_genTexture(outInfo, srcParam, RenderDataType::Texture2D);
				continue;
			}
			if (json_baseShape == "texture3D") {
				_genTexture(outInfo, srcParam, RenderDataType::Texture3D);
				continue;
			}
			if (json_baseShape == "structuredBuffer") { _genStructuredBuffer(outInfo, srcParam); continue; }
			throw Error_Undefined(Fmt("unsupported resource baseShape {}", json_baseShape));
		}

		throw Error_Undefined(Fmt("unsupported parameter data kind {}", *json_elementKind));
	}
}

void GenReflect_Slang::_genParamBase(ShaderStageInfo::ParamBase& dstParam, const ShaderStageInfo& info, const SrcParam& srcParam) {
	dstParam.name  = srcParam.parameter->member("name")->asString();
	dstParam.stageFlags = info.stageFlags;
	
	if (srcParam.binding->member("kind")->asString() == "pushConstantBuffer") {
		dstParam.bindPoint = BindPoint::Zero;
		dstParam.bindSpace = BindSpace::RootConst;
		dstParam.bindCount = 1;
	} else {
		auto* json_space = srcParam.binding->findMember("space");
		
		dstParam.bindSpace = json_space ? static_cast<BindSpace>(json_space->asInt64()) : BindSpace::Default;
		dstParam.bindPoint = static_cast<BindPoint>(srcParam.binding->member("index")->asInt64());
		
		if (srcParam.elementType) {
			dstParam.bindCount = ax_safe_cast_from(srcParam.type->member("elementCount")->asInt64());
		} else {
			dstParam.bindCount = 1;
		}
	}
}

void GenReflect_Slang::_genGlobalParam(ShaderStageInfo& outInfo, const SrcParam& srcParam) {
	auto& dstVar = _globalConstBuffer.variables.emplaceBack();
	auto& json_field_binding = srcParam.parameter->memberObject("binding");
	
	dstVar.name        = srcParam.parameter->memberString("name");
	dstVar.varType     = _getShaderVariableType(srcParam.parameter->memberObject("type"));
	dstVar.offset      = ax_safe_cast_from(json_field_binding.member("offset")->asInt64());
	dstVar.sizeInBytes = ax_safe_cast_from(json_field_binding.member("size")->asInt64());
}

void GenReflect_Slang::_genVariables(ShaderStageInfo::BufferBase& dstBuf, const JsonArray& json_fields) {
	dstBuf.stride = 0;
	dstBuf.variables.ensureCapacity(json_fields.size());
	for (auto& json_field : json_fields) {
		auto& dstVar = dstBuf.variables.emplaceBack();
		auto& json_field_binding = json_field.memberObject("binding");
		
		dstVar.name        = json_field.memberString("name");
		dstVar.varType     = _getShaderVariableType(json_field.memberObject("type"));
		dstVar.offset      = ax_safe_cast_from(json_field_binding.member("offset")->asInt64());
		dstVar.sizeInBytes = ax_safe_cast_from(json_field_binding.member("size")->asInt64());
		
		Math::max_itself(dstBuf.stride, dstVar.offset + dstVar.sizeInBytes);
	}
}

void GenReflect_Slang::_genSampler(ShaderStageInfo& outInfo, const SrcParam& srcParam) {
	auto& dstBuf = outInfo.samplers.emplaceBack();
	dstBuf.dataType = RenderDataType::SamplerState;
	_genParamBase(dstBuf, outInfo, srcParam);
}

void GenReflect_Slang::_genTexture(ShaderStageInfo& outInfo, const SrcParam& srcParam, RenderDataType dataType) {
	auto& dstBuf = outInfo.textures.emplaceBack();
	dstBuf.dataType = dataType;
	_genParamBase(dstBuf, outInfo, srcParam);
}

void GenReflect_Slang::_genStructuredBuffer(ShaderStageInfo& outInfo, const SrcParam& srcParam) {
	auto& dstBuf = outInfo.structuredBuffers.emplaceBack();
	dstBuf.dataType = RenderDataType::StructuredBuffer;
	
	auto& json_resultType = srcParam.type->memberObject("resultType");
	auto& json_fields     = json_resultType.memberArray("fields");
	
	_genParamBase(dstBuf, outInfo, srcParam);
	_genVariables(dstBuf, json_fields);
}

void GenReflect_Slang::_genConstBuffer(ShaderStageInfo& outInfo, const SrcParam& srcParam) {
	auto& dstBuf = outInfo.constBuffers.emplaceBack();
	dstBuf.dataType = RenderDataType::ConstBuffer;

	auto& json_elementVarLayout     = srcParam.type->memberObject("elementVarLayout");
	auto& json_elementVarLayoutType = json_elementVarLayout.memberObject("type");
	auto& json_fields               = json_elementVarLayoutType.memberArray("fields");
	
	_genParamBase(dstBuf, outInfo, srcParam);
	_genVariables(dstBuf, json_fields);
}

} // namespace

#endif