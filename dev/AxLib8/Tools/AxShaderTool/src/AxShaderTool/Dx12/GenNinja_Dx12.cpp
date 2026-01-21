module AxShaderTool;

#if AX_RENDERER_DX12
import :GenNinja_Dx12;

namespace ax {
void GenNinja_Dx12::writeNinjaPass(IString& outStr, IArray<String>& outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename) {
	auto& opt = CmdOptions::s_instance(); 
	
	outStr.append(	"rule build_Shader_Dx12_json\n"
					"  depfile = $out.d\n"
					"  command = \"${AxShaderTool}\" $\n"
					"    -genReflect_Dx12 $\n");

	if (opt.keepUnusedVariable) {
		outStr.append("    -keepUnusedVariable $\n");
	}

	outStr.append(	"    -profile=$param_profile $\n"
					"    -entry=$param_entry_point $\n"
					"    -I=\"$AxIncludeDir\" $\n"
					"    -out=\"$out\" $\n"
					"    -file=\"$in\""
					"\n\n"
					"rule build_Shader_Dx12_bin\n"
					"  command = \"$windows_sdk_bin/dxc\" $\n"
#if AX_RENDER_BINDLESS
					"    -DAX_RENDER_BINDLESS=1 $\n"
#endif
					"    -DAX_RENDER_DX12=1 $\n"
					"    -WX $\n" // Treat warnings as errors
					"    -T $param_profile $\n"
					"    -E $param_entry_point $\n"
					"    -I \"$AxIncludeDir\" $\n"
					"    -Fo \"$out\" \"$in\" $\n"
//					"    -Zpc $\n" // Pack matrices in column-major order
#if 1 // debug info
					"    -Zi $\n" // Enable debug information. Cannot be used together with -Zs
					"    -Qembed_debug $\n" // Embed PDB in shader container (must be used with /Zi)
#endif
					"\n\n");


	outStr.append(Fmt("SourceFile={}\n\n", relSourceFilename));

	auto writePass = [&](StrView entryPoint, ShaderStageFlags stageFlags, StrView profile) {
		if (!entryPoint) return;

		// Dependency:
		// - shaderResult.json       <-+ Shader_Dx12-{}-{}.bin.json.tmp <- Shader_Dx12-{}-{}.bin.json.tmp.d
		// - Shader_Dx12-{0}-{1}.bin <-+ same above 
		
		String outJsonFilename = Fmt("Shader_Dx12-{}-{}.bin.json.tmp", pass.name, stageFlags);
		outJsonFileList.append(outJsonFilename);

		outStr.append(Fmt("build {}: build_Shader_Dx12_json ${{SourceFile}} | ${{AxShaderTool}}\n", outJsonFilename, pass.name, stageFlags));
		outStr.append(Fmt("  param_entry_point = {}\n", entryPoint));
		outStr.append(Fmt("  param_profile     = {}\n", profile));
		outStr.append("\n");

		// have to use dxc.exe from MS, because that add validate hash in the bin file
		outStr.append(Fmt("build Shader_Dx12-{}-{}.bin: build_Shader_Dx12_bin ${{SourceFile}} | {} ${{AxShaderTool}} \n", pass.name, stageFlags, outJsonFilename));
		outStr.append(Fmt("  param_entry_point = {}\n", entryPoint));
		outStr.append(Fmt("  param_profile     = {}\n", profile));
		outStr.append("\n");
	};

	writePass(pass.vertexFunc       , ShaderStageFlags::Vertex       , "vs_6_5");
	writePass(pass.pixelFunc        , ShaderStageFlags::Pixel        , "ps_6_5");
	writePass(pass.geometryFunc     , ShaderStageFlags::Geometry     , "gs_6_5");
	writePass(pass.computeFunc      , ShaderStageFlags::Compute      , "cs_6_5");
	writePass(pass.meshFunc         , ShaderStageFlags::Mesh         , "ms_6_5");
	writePass(pass.amplificationFunc, ShaderStageFlags::Amplification, "as_6_5");
}

} // namespace
#endif // #if AX_RENDERER_DX12