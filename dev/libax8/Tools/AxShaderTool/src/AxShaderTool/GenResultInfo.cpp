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

		addStage(pass.vsFunc, ShaderStageFlags::Vertex  );
		addStage(pass.psFunc, ShaderStageFlags::Pixel   );
		addStage(pass.gsFunc, ShaderStageFlags::Geometry);
		addStage(pass.csFunc, ShaderStageFlags::Compute );
	}

	if (_resultInfo.declare.passes.size() != _resultInfo.passStages.size())
		throw Error_Undefined("ResultInfo declare passes count != passStages count");

	JsonIO::writeFile(outFilename, _resultInfo);
}

void GenResultInfo::mergeStageInfo(ShaderStageInfo& outStageInfo, const ShaderStageInfo& inStageInfo) {
	_mergeParam(outStageInfo.constBuffers, inStageInfo.constBuffers.span(),
		[&](auto& dst, auto& src){
			dst.dataType = src.dataType;
			dst.dataSize = src.dataSize;
		});

	_mergeParam(outStageInfo.textures, inStageInfo.textures.span(),
		[&](auto& dst, auto& src){
			dst.dataType = src.dataType;
			dst.dataSize = src.dataSize;
		});

	_mergeParam(outStageInfo.samplers, inStageInfo.samplers.span(),
		[&](auto& dst, auto& src){
			dst.dataType = src.dataType;
			dst.dataSize = src.dataSize;
		});

	_mergeParam(outStageInfo.storageBuffers, inStageInfo.storageBuffers.span(),
		[&](auto& dst, auto& src){
			dst.dataType = src.dataType;
			dst.dataSize = src.dataSize;
		});

	outStageInfo.stageFlags |= inStageInfo.stageFlags;
}

template<class T, class FUNC> inline
void GenResultInfo::_mergeParam(IArray<T>& dstArray, Span<T> srcSpan, FUNC func) {
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
	}
}



} // namespace
