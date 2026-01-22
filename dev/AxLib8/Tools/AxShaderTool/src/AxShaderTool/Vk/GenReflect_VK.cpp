#if 0

module;

#if AX_RENDERER_VK

// AX_GCC_WARNING_PUSH_AND_DISABLE("-Wmicrosoft-enum-value") // INVALID_VALUE  = 0xFFFFFFFF,
// AX_VC_WARNING_PUSH_AND_DISABLE(5039)
// Warning C5039 : 'qsort': pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc.
// Undefined behavior may occur if this function throws an exception.
// AX_VC_WARNING_PUSH_AND_DISABLE(5262)
// Warning C5262 : implicit fall-through occurs here; are you missing a break statement? Use [[fallthrough]]
// when a break statement is intentionally omitted between cases

#include "spirv_reflect.c"

// AX_VC_WARNING_POP()
// AX_VC_WARNING_POP()
// AX_GCC_WARNING_POP()

module AxShaderTool;
import :GenReflect_Vk;

namespace ax /*::AxRender*/ {

struct VarNameToSemantic {

	static VertexSemantic s_get(StrView name) {
		static VarNameToSemantic s;
		auto* p = s._dict.find(name);
		if (!p) {
			return VertexSemantic::None; // TODO
			throw Error_Undefined(Fmt("Cannot resolve vertex semantic from name '{}'", name));
		}
		return *p;
	}

private:
	Dict<String, VertexSemantic>	_dict;

	void _add(StrView name, VertexSemantic s, u16 count) {
		for (u16 i = 0; i < count; i++) {
			_dict.add(Fmt("{}{}", name, i), s + i);
		}
	}

	VarNameToSemantic() {
		_dict.add("i.sv_pos"                   ,  VertexSemantic::SV_POSITION);
		_dict.add("i.sv_depth"                 ,  VertexSemantic::SV_DEPTH);
		_dict.add("i.sv_coverage"              ,  VertexSemantic::SV_COVERAGE);
		_dict.add("i.sv_vertexId"              ,  VertexSemantic::SV_VERTEXID);
		_dict.add("i.sv_primitiveId"           ,  VertexSemantic::SV_PRIMITIVEID);
		_dict.add("i.sv_instanceId"            ,  VertexSemantic::SV_INSTANCEID);
		_dict.add("i.sv_dispatchThreadId"      ,  VertexSemantic::SV_DISPATCHTHREADID);
		_dict.add("i.sv_groupId"               ,  VertexSemantic::SV_GROUPID);
		_dict.add("i.sv_groupIndex"            ,  VertexSemantic::SV_GROUPINDEX);
		_dict.add("i.sv_groupThreadId"         ,  VertexSemantic::SV_GROUPTHREADID);
		_dict.add("i.sv_gsInstanceId"          ,  VertexSemantic::SV_GSINSTANCEID);
	//----
		_dict.add("i.pos",	VertexSemantic::POSITION);
	//----
		_add("i.uv",		VertexSemantic::TEXCOORD0,	20);
		_add("i.color",		VertexSemantic::COLOR0,		8);
		_add("i.normal",	VertexSemantic::NORMAL0,	8);
		_add("i.tangent",	VertexSemantic::TANGENT0,	8);
		_add("i.binormal",	VertexSemantic::BINORMAL0,	8);
	}
};

ShaderStageFlags getShaderStageFlags(SpvReflectShaderStageFlagBits f) {
	using DST = ShaderStageFlags;
	DST o = DST::None;
	if (ax_bit_has(f, SPV_REFLECT_SHADER_STAGE_VERTEX_BIT  )) o |= DST::Vertex;
	if (ax_bit_has(f, SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT)) o |= DST::Pixel;
	if (ax_bit_has(f, SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT)) o |= DST::Geometry;
	if (ax_bit_has(f, SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT )) o |= DST::Compute;
	return static_cast<ShaderStageFlags>(o);
}

RenderDataType getDataType(const SpvReflectTypeDescription& src) {
	auto& numeric = src.traits.numeric;
	auto& scalar  = numeric.scalar;

	if (src.type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX) {
		auto& mat = numeric.matrix;

		if (src.type_flags & SPV_REFLECT_TYPE_FLAG_BOOL) {
			AX_ASSERT(false);
			throw Error_Undefined("unsupported bool-matrix");
		}
		if (src.type_flags & SPV_REFLECT_TYPE_FLAG_INT) {
			AX_ASSERT(false);
			throw Error_Undefined("unsupported int-matrix");
		}
		if (src.type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) {
			if (mat.row_count == 4 && mat.column_count == 4) {
				switch (scalar.width) {
					case 32: return RenderDataType::Mat4f;
					case 64: return RenderDataType::Mat4d;
				}
			}
		}
	}

	auto& vec = numeric.vector;

	if (src.type_flags & SPV_REFLECT_TYPE_FLAG_INT) {
		switch (scalar.width) {
			case 8: switch (vec.component_count) {
				case 0: return scalar.signedness ? RenderDataType::i8     : RenderDataType::u8;
				case 1: return scalar.signedness ? RenderDataType::Vec1i8 : RenderDataType::Vec1u8;
				case 2: return scalar.signedness ? RenderDataType::Vec2i8 : RenderDataType::Vec2u8;
				case 3: return scalar.signedness ? RenderDataType::Vec3i8 : RenderDataType::Vec3u8;
				case 4: return scalar.signedness ? RenderDataType::Vec4i8 : RenderDataType::Vec4u8;
			} break;
			case 16: switch (vec.component_count) {
				case 0: return scalar.signedness ? RenderDataType::i16     : RenderDataType::u16;
				case 1: return scalar.signedness ? RenderDataType::Vec1i16 : RenderDataType::Vec1u16;
				case 2: return scalar.signedness ? RenderDataType::Vec2i16 : RenderDataType::Vec2u16;
				case 3: return scalar.signedness ? RenderDataType::Vec3i16 : RenderDataType::Vec3u16;
				case 4: return scalar.signedness ? RenderDataType::Vec4i16 : RenderDataType::Vec4u16;
			} break;
			case 32: switch (vec.component_count) {
				case 0: return scalar.signedness ? RenderDataType::i32     : RenderDataType::u32;
				case 1: return scalar.signedness ? RenderDataType::Vec1i32 : RenderDataType::Vec1u32;
				case 2: return scalar.signedness ? RenderDataType::Vec2i32 : RenderDataType::Vec2u32;
				case 3: return scalar.signedness ? RenderDataType::Vec3i32 : RenderDataType::Vec3u32;
				case 4: return scalar.signedness ? RenderDataType::Vec4i32 : RenderDataType::Vec4u32;
			} break;
			case 64: switch (vec.component_count) {
				case 0: return scalar.signedness ? RenderDataType::i64     : RenderDataType::u64;
				case 1: return scalar.signedness ? RenderDataType::Vec1i64 : RenderDataType::Vec1u64;
				case 2: return scalar.signedness ? RenderDataType::Vec2i64 : RenderDataType::Vec2u64;
				case 3: return scalar.signedness ? RenderDataType::Vec3i64 : RenderDataType::Vec3u64;
				case 4: return scalar.signedness ? RenderDataType::Vec4i64 : RenderDataType::Vec4u64;
			} break;
		}
	} else if (src.type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) {
		switch (scalar.width) {
			case 16: switch (vec.component_count) {
				case 0: return RenderDataType::f16;
				case 1: return RenderDataType::Vec1h;
				case 2: return RenderDataType::Vec1h;
				case 3: return RenderDataType::Vec1h;
				case 4: return RenderDataType::Vec1h;
			} break;
			case 32: switch (vec.component_count) {
				case 0: return RenderDataType::f32;
				case 1: return RenderDataType::Vec1f;
				case 2: return RenderDataType::Vec2f;
				case 3: return RenderDataType::Vec3f;
				case 4: return RenderDataType::Vec4f;
			} break;
			case 64: switch (vec.component_count) {
				case 0: return RenderDataType::f64;
				case 1: return RenderDataType::Vec1d;
				case 2: return RenderDataType::Vec2d;
				case 3: return RenderDataType::Vec3d;
				case 4: return RenderDataType::Vec4d;
			} break;
		}
	} else if (src.type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT) {
		return RenderDataType::Struct;
	}

	AX_ASSERT(false);
	return RenderDataType::None;
}

void GenReflect_Vk::generate(StrView outFilename, StrView filename, RenderAPI api) {
	FileMemMap	spv_data(filename);

	spv_reflect::ShaderModule spirvReflect(spv_data.size(), spv_data.data());
	_throwIfError(spirvReflect.GetResult());

	ShaderStageInfo outInfo;
	outInfo.stageFlags = getShaderStageFlags(spirvReflect.GetShaderStage());

	_genVertexInputs	(outInfo, spirvReflect);
	_genPushConstants	(outInfo, spirvReflect);
	_genBindings		(outInfo, spirvReflect);

	auto& opt = CmdOptions::s_instance();
	JsonIO::writeFile(outFilename, outInfo, opt.writeFileOpt);
}

void GenReflect_Vk::_genVertexInputs(ShaderStageInfo& outInfo, const spv_reflect::ShaderModule& spirvReflect) {

	u32 count;
	spirvReflect.EnumerateInputVariables(&count, nullptr);

	Array<SpvReflectInterfaceVariable*, 32>	inputVars;
	inputVars.resize(count);
	spirvReflect.EnumerateInputVariables(&count, inputVars.data());
	
	outInfo.inputs.ensureCapacity(inputVars.size());

	for (auto& v : inputVars) {
		auto& dst = outInfo.inputs.emplaceBack();
		dst.dataType = getDataType(*v->type_description);
		dst.semantic = VarNameToSemantic::s_get(StrView_c_str(v->name));
	}
}

void GenReflect_Vk::_genPushConstants(ShaderStageInfo& outInfo, const spv_reflect::ShaderModule& spirvReflect) {
	u32 count;
	spirvReflect.EnumeratePushConstantBlocks(&count, nullptr);
	Array<SpvReflectBlockVariable*, 1> blocks;
	blocks.resize(count);
	spirvReflect.EnumeratePushConstantBlocks(&count, blocks.data());
	
	for (auto& block : blocks) {
		auto& dstCB      = outInfo.constBuffers.emplaceBack();
		dstCB.name       = StrView_c_str(block->type_description->type_name);
		dstCB.dataSize   = block->size;
		dstCB.bindPoint  = BindPoint::Zero;
		dstCB.bindCount  = 1;
		dstCB.bindSpace  = ShaderParamBindSpace::RootConst;
		dstCB.stageFlags = getShaderStageFlags(spirvReflect.GetShaderStage());

		for (u32 i = 0; i < block->member_count; i++) {
			auto& srcVar	= block->members[i];
			auto& dstVar	= dstCB.variables.emplaceBack();
			dstVar.name		= StrView_c_str(srcVar.name);
			dstVar.offset	= srcVar.offset;
			dstVar.dataType = getDataType(*srcVar.type_description);
		}		
	}
}

void GenReflect_Vk::_genBindings(ShaderStageInfo& outInfo, const spv_reflect::ShaderModule& spirvReflect) {
	u32 count;
	spirvReflect.EnumerateDescriptorBindings(&count, nullptr);

	Array<SpvReflectDescriptorBinding*, 32> descriptorBindings;
	descriptorBindings.resize(count);

	spirvReflect.EnumerateDescriptorBindings(&count, descriptorBindings.data());

	for (auto& binding : descriptorBindings) {

		switch (binding->descriptor_type) {
			case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:		_genSampler(outInfo, binding); break;
//			case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: break;
			case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:	_genTexture(outInfo, binding); break;

//			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:			break;
//			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:	break;
//			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:	break;
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: _genConstBuffer(outInfo, binding); break;
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: _genStorageBuffer(outInfo, binding); break;
//			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: break;
//			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: break;
//			case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: break;
//			case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR: break;

			default: {
				AX_LOG_WARNING("unhandled binding->descriptor_type {}", static_cast<int>(binding->descriptor_type));
			} break;
		}
	}
}

template<class PARAM>
void GenReflect_Vk::_genParamBase(PARAM& dst, ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding) {
	dst.stageFlags = outInfo.stageFlags;
	dst.name  = StrView_c_str(binding->name);
	dst.bindSpace = ax_safe_cast_from(binding->set);
	dst.bindPoint = ax_safe_cast_from(binding->binding);

	if (binding->array.dims_count == 1) {
		dst.bindCount = ax_safe_cast_from(binding->array.dims[0]);
	} else {
		dst.bindCount = 1;
	}
}

void GenReflect_Vk::_genSampler(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding) {
	auto& dst = outInfo.samplers.emplaceBack();
	_genParamBase(dst, outInfo, binding);
	dst.dataType = RenderDataType::SamplerState;
}

void GenReflect_Vk::_genTexture(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding) {
	auto& dst = outInfo.textures.emplaceBack();
	_genParamBase(dst, outInfo, binding);

	auto& img = binding->image;
	switch (img.dim) {
		case SpvDim1D:   dst.dataType = RenderDataType::Texture1D;   break;
		case SpvDim2D:   dst.dataType = RenderDataType::Texture2D;   break;
		case SpvDim3D:   dst.dataType = RenderDataType::Texture3D;   break;
		case SpvDimCube: dst.dataType = RenderDataType::TextureCube; break;
		default: throw Error_Undefined();
	}
}

void GenReflect_Vk::_genStorageBuffer(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding) {
	auto& dst = outInfo.structuredBuffers.emplaceBack();
	_genParamBase(dst, outInfo, binding);
	dst.dataType = RenderDataType::StructuredBuffer;
	dst.name = StrView_c_str(binding->name);

	if (binding->block.member_count < 1)
		throw Error_Undefined();
		
	auto& block = binding->block.members[0];
	dst.stride = ax_safe_cast_from(block.size);
	u32 memberCount = block.member_count;
	
	for (u32 i = 0; i < memberCount; i++) {
		auto& srcVar	= block.members[i];
		auto& dstVar	= dst.variables.emplaceBack();
		dstVar.name		= StrView_c_str(srcVar.name);
		dstVar.offset	= srcVar.offset;
		dstVar.dataType = getDataType(*srcVar.type_description);
	}
}

void GenReflect_Vk::_genConstBuffer(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding) {
	auto& dst = outInfo.constBuffers.emplaceBack();
	_genParamBase(dst, outInfo, binding);
	dst.dataType = RenderDataType::ConstBuffer;
	dst.name = StrView_c_str(binding->name);

	auto& block = binding->block;
	dst.dataSize = ax_safe_cast_from(block.size);

	u32 memberCount = block.member_count;
	for (u32 i = 0; i < memberCount; i++) {
		auto& srcVar	= block.members[i];
		auto& dstVar	= dst.variables.emplaceBack();
		dstVar.name		= StrView_c_str(srcVar.name);
		dstVar.offset	= srcVar.offset;
		dstVar.dataType = getDataType(*srcVar.type_description);
	}
}

} // namespace

#endif

#endif
