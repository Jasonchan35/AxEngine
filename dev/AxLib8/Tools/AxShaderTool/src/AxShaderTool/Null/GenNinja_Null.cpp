module AxShaderTool;

#if AX_RENDERER_NULL

import :GenNinja_Null;

namespace ax {

void GenNinja_Null::writeNinjaPass(IString& outStr, IArray<String>& outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename) {
	outStr.append(	"rule build_Shader_Null_json\n"
					"  command = \"${AxShaderTool}\" $\n"
					"    -genReflect_Null $\n"
					"    -out=\"$out\" $\n"
					"\n\n");

	outStr.append(Fmt("SourceFile={}\n\n", relSourceFilename));

	
	auto writePass = [&](StrView entryPoint, ShaderStageFlags stageFlags, StrView profile) {
		if (!entryPoint) return;

		String outJsonFilename = Fmt("Shader_Null-{0}-{1}.bin.json.tmp", pass.name, stageFlags);
		outJsonFileList.append(outJsonFilename);

		outStr.append(Fmt("build {}: build_Shader_Null_json | ${{AxShaderTool}}\n", outJsonFilename));
		outStr.append("\n");
	};
	
	
	writePass(pass.vsFunc, ShaderStageFlags::Vertex  , "vertex"  );
	writePass(pass.psFunc, ShaderStageFlags::Pixel   , "fragment");
	writePass(pass.gsFunc, ShaderStageFlags::Geometry, "geometry");
	writePass(pass.csFunc, ShaderStageFlags::Compute , "compute" );
}

} // namespace

#endif // #if AX_RENDERER_NULL