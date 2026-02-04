module AxShaderTool;

#if AX_RENDER_VK

import :GenNinja_Vk;

namespace ax {

void GenNinja_Vk::writeNinjaPass(IString& outStr, IArray<String>& outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename) {
	outStr.append(	"rule build_Shader_Vk_bin\n"
					"  depfile = $out.d\n"
					"  command = \"$vulkan_sdk/bin/slangc\" $\n"
					"    -lang hlsl $\n"
					"    -stage $param_shader_stage $\n"
					"    -profile sm_6_5 $\n"
					"    -target spirv $\n"
					"    -entry $param_entry_point $\n"
					"    -I \"$AxIncludeDir\" $\n"
					"    -DAX_SHADER_INFO=0 $\n"
					"    -DAX_RENDER_VK=1 $\n"
#if AX_RENDER_BINDLESS
					"    -DAX_RENDER_BINDLESS=1 $\n"
#endif
					"    -warnings-as-errors all $\n");
	
	for (auto bindSpace : Range_(ShaderParamBindSpace::_COUNT)) {
		outStr.appendFormat("    -fvk-b-shift 0     {} $\n", ax_enum_int(bindSpace)); // Constant buffer view
		outStr.appendFormat("    -fvk-s-shift 10000 {} $\n", ax_enum_int(bindSpace)); // Sampler
		outStr.appendFormat("    -fvk-t-shift 20000 {} $\n", ax_enum_int(bindSpace)); // Shader resource view
		outStr.appendFormat("    -fvk-u-shift 30000 {} $\n", ax_enum_int(bindSpace)); // Unordered access view
	}
	
	outStr.append(	"    -depfile \"$out.d\" $\n"
					"    -reflection-json \"$out.reflect\" $\n"
					"    -fvk-use-entrypoint-name $\n"
					"    -o \"$out\" $\n"
					"    -- $in $\n"
					"\n\n");
	
	
#if 0
	outStr.append(	"rule build_Shader_Vk_bin\n"
					"  depfile = $out.d\n"
					"  command = \"$vulkan_sdk/Bin/glslc\" $\n"
					"    -x hlsl $\n"
					"    --target-env=vulkan1.4 $\n" // Ensures support for the vk:: attributes
					"    -fshader-stage=$param_shader_stage $\n"
					"    -fentry-point=$param_entry_point $\n"
					"    -fauto-bind-uniforms $\n"
					//"    -fubo-binding-base 16 $\n"
					"    -fsampler-binding-base 4 $\n"
					"    -ftexture-binding-base 4 $\n"
					"      -fimage-binding-base 4 $\n"
					"        -fuav-binding-base 4 $\n"
					"       -fssbo-binding-base 4 $\n"
#if AX_RENDER_BINDLESS
					"    -DAX_RENDER_BINDLESS=1 $\n"
#endif
					"    -DAX_RENDER_VK=1 $\n"
					"    -Werror $\n" // Treat warnings as errors
					"    -MD -MF \"$out.d\" $\n"
					"    -I \"$AxIncludeDir\" $\n"
					"    -o \"$out\" $in $\n"
					"\n\n");
#endif
	
#if 0
	outStr.append(	"rule build_Shader_Vk_reflect\n"
					"  command = \"$vulkan_sdk/Bin/spirv-cross\" $\n"
					"    --reflect $\n"
					"    --remove-unused-variables $\n"
					"    --hlsl-auto-binding sampler $\n" // assign register id "Texture2D NAME : register(t ## REG); "
					"    --set-hlsl-vertex-input-semantic 100 POSITION $\n"
					"    --output \"$out\" $in"
					"\n\n");
#endif

	outStr.append(	"rule build_Shader_Vk_json\n"
					"  command = \"${AxShaderTool}\" $\n"
					"    -genReflect_Vk $\n"
					"    -file=$in $\n"
					"    -out=\"$out\"  $\n"
					"\n\n");

	outStr.append(Fmt("SourceFile={}\n\n", AxNinjaBuild::escapeString(relSourceFilename)));

	auto writePass = [&](StrView entryPoint, ShaderStageFlags stageFlags, StrView profile) {
		if (!entryPoint) return;

		outStr.append(Fmt(	"build Shader_Vk-{0}-{1}.bin | Shader_Vk-{0}-{1}.bin.reflect: "
							"build_Shader_Vk_bin ${{SourceFile}} | ${{AxShaderTool}}\n", pass.name, stageFlags));
		outStr.append(Fmt("  param_shader_stage = {}\n", profile));
		outStr.append(Fmt("  param_entry_point  = {}\n", entryPoint));
		outStr.append("\n");

#if 0
		outStr.append(Fmt("build Shader_Vk-{0}-{1}.reflect.json.tmp: build_Shader_Vk_reflect VK-{0}-{1}.bin\n", pass.name, stageFlags));
		outStr.append("\n");
#endif

		String outJsonFilename = Fmt("Shader_Vk-{0}-{1}.bin.json.tmp", pass.name, stageFlags);
		outJsonFileList.append(outJsonFilename);

		outStr.append(Fmt("build {}: build_Shader_Vk_json Shader_Vk-{}-{}.bin.reflect | ${{AxShaderTool}}\n", outJsonFilename, pass.name, stageFlags));
		outStr.append("\n");
	};

	writePass(pass.vertexFunc       , ShaderStageFlags::Vertex       , "vertex"   );
	writePass(pass.pixelFunc        , ShaderStageFlags::Pixel        , "fragment" );
	writePass(pass.geometryFunc     , ShaderStageFlags::Geometry     , "geometry" );
	writePass(pass.computeFunc      , ShaderStageFlags::Compute      , "compute"  );
	writePass(pass.meshFunc         , ShaderStageFlags::Mesh         , "mesh"     );
	writePass(pass.amplificationFunc, ShaderStageFlags::Amplification, "task"     );
}

} // namespace

#endif // #if AX_RENDER_VK