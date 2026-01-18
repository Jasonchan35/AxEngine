module AxShaderTool;

#if AX_RENDERER_VK

import :GenNinja_Vk;

namespace ax {

void GenNinja_Vk::writeNinjaPass(IString& outStr, IArray<String>& outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename) {
	outStr.append(	"rule build_Shader_Vk_bin\n"
					"  depfile = $out.d\n"
					"  command = \"$vulkan_sdk/Bin/glslc\" $\n"
					"    -x hlsl $\n"
					"    --target-env=vulkan1.2 $\n" // Ensures support for the vk:: attributes
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
					"    -MD -MF \"$out.d\""
					"    -I \"$AxIncludeDir\""
					"    -o \"$out\" \"$in\""
					"\n\n");

#if 0
	outStr.append(	"rule build_Shader_Vk_reflect\n"
					"  command = \"$vulkan_sdk/Bin/spirv-cross\" $\n"
					"    --reflect $\n"
					"    --remove-unused-variables $\n"
					"    --hlsl-auto-binding sampler $\n" // assign register id "Texture2D NAME : register(t ## REG); "
					"    --set-hlsl-vertex-input-semantic 100 POSITION $\n"
					"    --output \"$out\" \"$in\""
					"\n\n");
#endif

	outStr.append(	"rule build_Shader_Vk_json\n"
					"  command = \"${AxShaderTool}\" $\n"
					"    -genReflect_Vk $\n"
					"    -file=\"$in\"  $\n"
					"    -out=\"$out\"  $\n"
					"\n\n");

	outStr.append(Fmt("SourceFile={}\n\n", relSourceFilename));

	auto writePass = [&](StrView entryPoint, ShaderStageFlags stageFlags, StrView profile) {
		if (!entryPoint) return;

		outStr.append(Fmt("build Shader_Vk-{0}-{1}.bin: build_Shader_Vk_bin ${{SourceFile}} | ${{AxShaderTool}}\n", pass.name, stageFlags));
		outStr.append(Fmt("  param_shader_stage = {}\n", profile));
		outStr.append(Fmt("  param_entry_point  = {}\n", entryPoint));
		outStr.append("\n");

#if 0
		outStr.append(Fmt("build Shader_Vk-{0}-{1}.reflect.json.tmp: build_Shader_Vk_reflect VK-{0}-{1}.bin\n", pass.name, stageFlags));
		outStr.append("\n");
#endif

		String outJsonFilename = Fmt("Shader_Vk-{0}-{1}.bin.json.tmp", pass.name, stageFlags);
		outJsonFileList.append(outJsonFilename);

		outStr.append(Fmt("build {}: build_Shader_Vk_json Shader_Vk-{}-{}.bin | ${{AxShaderTool}}\n", outJsonFilename, pass.name, stageFlags));
		outStr.append("\n");
	};

	writePass(pass.vsFunc, ShaderStageFlags::Vertex  , "vertex"  );
	writePass(pass.psFunc, ShaderStageFlags::Pixel   , "fragment");
	writePass(pass.gsFunc, ShaderStageFlags::Geometry, "geometry");
}

} // namespace

#endif // #if AX_RENDERER_VK