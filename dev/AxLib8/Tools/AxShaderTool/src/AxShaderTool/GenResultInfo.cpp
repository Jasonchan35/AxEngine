module AxShaderTool;

import :GenResultInfo;

namespace ax /*::AxRender*/ {

void GenResultInfo::run(StrView outFilename, StrView filename, RenderAPI api) {
	JsonIO::readFile(filename, _resultInfo.declare);

	_resultInfo.passStages.ensureCapacity(_resultInfo.declare.passes.size());

	auto apiPath = Fmt("{}/{}", filename, api);


	for (auto& pass : _resultInfo.declare.passes) {
		auto& outStageInfo = _resultInfo.passStages.emplaceBack();

		auto addStage = [&](StrView funcName, ShaderStageFlags stageFlags) {
			if (!funcName) return;

			auto stageInfoFilename = Fmt("Shader_{}-{}-{}.bin.json.tmp", api, pass.name, stageFlags);

			ShaderStageInfo	inStageInfo;
			JsonIO::readFile(stageInfoFilename, inStageInfo);

			if (stageFlags == ShaderStageFlags::Vertex) {
				outStageInfo.inputs = inStageInfo.inputs;
			}

			mergeStageInfo(outStageInfo, inStageInfo);
		};

		addStage(pass.vertexFunc       , ShaderStageFlags::Vertex       );
		addStage(pass.pixelFunc        , ShaderStageFlags::Pixel        );
		addStage(pass.geometryFunc     , ShaderStageFlags::Geometry     );
		addStage(pass.computeFunc      , ShaderStageFlags::Compute      );
		addStage(pass.meshFunc         , ShaderStageFlags::Mesh         );
		addStage(pass.amplificationFunc, ShaderStageFlags::Amplification);
	}

	if (_resultInfo.declare.passes.size() != _resultInfo.passStages.size())
		throw Error_Undefined("ResultInfo declare passes count != passStages count");

	JsonIO::writeFile(outFilename, _resultInfo);
}

void GenResultInfo::mergeStageInfo(ShaderStageInfo& outStageInfo, const ShaderStageInfo& inStageInfo) {
	_mergeParam(outStageInfo.constBuffers,      inStageInfo.constBuffers.span());
	_mergeParam(outStageInfo.textures,          inStageInfo.textures.span());
	_mergeParam(outStageInfo.samplers,          inStageInfo.samplers.span());
	_mergeParam(outStageInfo.structuredBuffers, inStageInfo.structuredBuffers.span());

	outStageInfo.stageFlags |= inStageInfo.stageFlags;
}

void GenResultInfo::_mergeVariables(IArray<Variables>& dstArray, Span<Variables> srcSpan) {
	for (auto& src : srcSpan) {
		auto f = dstArray.find_([&](const auto& e){ return e.name == src.name; });
		if (f) {
			if (f->dataType != src.dataType) throw Error_Runtime(Fmt("variable'{}' dataType mismatch", src.name));
			if (f->offset   != src.offset  ) throw Error_Runtime(Fmt("variable'{}' offset mismatch", src.name));
			if (f->size     != src.size    ) throw Error_Runtime(Fmt("variable'{}' size mismatch", src.name));
			continue;
		}
		dstArray.emplaceBack(src);
	}
}

template<class T> inline
void GenResultInfo::_mergeParam(IArray<T>& dstArray, Span<T> srcSpan) {
	for (auto& src : srcSpan) {

		if (!_resultInfo.declare.isGlobalCommonShader) {
			if (src.bindSpace != ShaderParamBindSpace::Default) continue;
		}

		auto* dst = _findParam(dstArray.span(), src.name);
		if (dst) {
			if (dst->bindPoint != src.bindPoint) throw Error_Runtime(Fmt("param'{}' bindPoint mismatch", src.name));
			if (dst->bindCount != src.bindCount) throw Error_Runtime(Fmt("param'{}' bindCount mismatch", src.name));
			if (dst->bindSpace != src.bindSpace) throw Error_Runtime(Fmt("param'{}' bindSpace mismatch", src.name));

		} else {
			dst = &dstArray.emplaceBack(src);
		}

		dst->stageFlags |= src.stageFlags;

		if constexpr (Type_IsSame<T, ConstBuffer>) {
			_mergeVariables(dst->variables, src.variables);
		}
	}
}



} // namespace
