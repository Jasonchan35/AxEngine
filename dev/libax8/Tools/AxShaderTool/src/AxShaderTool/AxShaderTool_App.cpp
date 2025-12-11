module AxShaderTool;
import :App;
import :GenReflect_DX12;
import :GenReflect_VK;
import :ShaderInfoParser;
import :GenResultInfo;

namespace ax::AxRender {

AxShaderTool_App::AxShaderTool_App() {
	Error::s_setEnableAssertion(true);
}

void AxShaderTool_App::genNinja_ShadersInFolder(StrView outdir, StrView filename) {
	Array<String>	outFiles;
	File::glob(Fmt("{}/**/*.axShader", filename),
		[&](auto& entry) {
			auto relFile = FilePath::relPath(entry.fullpath, filename);
			auto relOutDir = Fmt("{}/{}", outdir, relFile);
			outFiles.emplaceBack(relFile);
			genNinja_Shader(relOutDir, entry.fullpath);
		
		});

	
	File::glob(Fmt("{}/**/*.axComputeShader", filename),
		[&](auto& entry) {
			auto relFile = FilePath::relPath(entry.fullpath, filename);
			auto relOutDir = Fmt("{}/{}", outdir, relFile);
			outFiles.emplaceBack(relFile);

			genNinja_Shader(relOutDir, entry.fullpath);
		});

	genNinja_Shaders(outdir, outFiles);
}

void AxShaderTool_App::genNinja_Shaders(StrView outdir, const Array<String>& files) {
	String outStr;
	writeNinja_Header(outStr);

	outStr.append(	"rule build_shader\n"
				// "  command = ninja -C $in -t targets depth 10\n"
				"  command = ninja --quiet -C $in\n"
				"\n");

	for (auto& f : files) {
		auto absFilename = FilePath::absPath(f);
		outStr.append(Fmt("build {}/shaderResult.json: build_shader {} | ${{AxShaderTool}} \n", f, f));
	}

	File::writeFileIfChanged(Fmt("{}/build.ninja", outdir), outStr, true);
}

void AxShaderTool_App::genNinja_Shader(StrView outdir, StrView filename) {
	if (!outdir) {
		AX_LOG("error: please specify outdir");
		throw Error_Undefined();
	}

	if (FileDir::exists(filename)) { // if the input is a directory
		genNinja_ShadersInFolder(outdir, filename);
		return;
	}

	String outStr;
	writeNinja_Header(outStr);

	outStr.append("rule build_api_shader\n");
	if (opt.quiet) {
		outStr.append("  command = ninja --quiet -C $in\n\n");
	} else {
	// outStr.append("  command = ninja -C $in -t targets depth 10\n\n")
		outStr.append("  command = ninja -C $in\n\n");
	}


	ShaderInfoParser parser;
	parser.readFile(outdir, filename);

	opt.keepUnusedVariable = parser.info.isGlobalCommonShader;

	outStr.append(Fmt("SourceFile={}\n\n", filename));

	auto func = [&](RenderApi api) {
		outStr.append(Fmt("build {}/shaderResult.json: build_api_shader {} | ${{AxShaderTool}} ${{SourceFile}}\n", api, api));
		genNinja_Shader_API(api, parser.info, outdir, filename);
	};

#if AX_RENDERER_NULL
	func(RenderApi::Null);
#endif
#if AX_RENDERER_VK	
	func(RenderApi::VK);
#endif
#if AX_RENDERER_DX12
	func(RenderApi::DX12);
#endif

	auto outFilename = Fmt("{}/build.ninja", outdir);
	File::writeFileIfChanged(outFilename, outStr, true);
}

void AxShaderTool_App::writeNinja_Header(IString& outStr) {
	auto writeEnvVar = [&](ZStrView name) {
		auto env = System::envVariable(name);
		if (!env) {
			AX_LOG("Env Var \"{}\" not found", name);
			return;
		}

		if (!env.value()) {
			AX_LOG("Env Var \"{}\" is empty", name);
			return;
		}

		auto v = FilePath::absPath(env.value());
		outStr.append(Fmt("{}={}\n", name, v));
	};

	writeEnvVar("ninja");
	writeEnvVar("vulkan_sdk");
	writeEnvVar("windows_sdk_bin");
	writeEnvVar("AxEngine_bin");
	writeEnvVar("AxIncludeDir");
	writeEnvVar("AxShaderTool");
	outStr.append("\n\n");
}

void AxShaderTool_App::genNinja_Shader_API(RenderApi api, ShaderDeclareInfo& info, StrView outdir, StrView filename) {	
	auto apiOutdir = Fmt("{}/{}", outdir, api);

	auto absSourceFilename = FilePath::absPath(filename);

	String outStr;
	writeNinja_Header(outStr);

	Array<String, 64>	depFileList;
//---
	for (auto& pass : info.passes) {
		switch (api) {
#if AX_RENDERER_NULL
			case RenderApi::Null:   writeNinja_NullPass(outStr, depFileList, pass, absSourceFilename); break;
#endif
#if AX_RENDERER_VK
			case RenderApi::VK:		writeNinja_VulkanPass(outStr, depFileList, pass, absSourceFilename); break;
#endif
#if AX_RENDERER_DX12
			case RenderApi::DX12:	writeNinja_DX12Pass  (outStr, depFileList, pass, absSourceFilename); break;
#endif
			default: break;
		}
	}

	outStr.append(	"rule build_result_info\n"
					"  command = ${AxShaderTool} $\n"
					"    -genResultInfo $\n"
					"    -api=$param_api $\n"
					"    -file=\"$in\" $\n"
					"    -out=\"$out\" $\n"
					"\n\n");

	outStr.append("build shaderResult.json: build_result_info ../info.json | ${AxShaderTool} ");
	for (auto& f : depFileList) {
		outStr.append(Fmt(" {}", f));
	}
	outStr.append("\n");
	outStr.append(Fmt("  param_api  = {}\n ", api      ));
	outStr.append("\n\n");

	auto outFilename = Fmt("{}/build.ninja", apiOutdir);
	File::writeFileIfChanged(outFilename, outStr, true);
}

#if AX_RENDERER_DX12
void AxShaderTool_App::writeNinja_DX12Pass(IString& outStr, IArray<String>& outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename) {

	outStr.append(	"#---- DX12 ----\n"
					"rule build_dx12_bin_json\n"
					"  depfile = $out.d\n"
					"  command = \"${AxShaderTool}\" $\n"
					"    -genReflect_DX12 $\n");

	if (opt.keepUnusedVariable) {
		outStr.append("    -keepUnusedVariable $\n");
	}

	outStr.append(	"    -profile=$param_profile $\n"
					"    -entry=$param_entry_point $\n"
					"    -I=\"$AxIncludeDir\" $\n"
					"    -out=\"$out\" $\n"
					"    -file=\"$in\""
					"\n\n"
					"rule build_dx12_bin\n"
					"  command = \"$windows_sdk_bin/dxc\" $\n"
#if AX_RENDER_BINDLESS
					"    -DAX_RENDER_BINDLESS=1 $\n"
#endif
					"    -WX $\n" // Treat warnings as errors
					"    -T $param_profile $\n"
					"    -E $param_entry_point $\n"
					"    -I \"$AxIncludeDir\" $\n"
					"    -Fo \"$out\" \"$in\""
					"\n\n");


	outStr.append(Fmt("SourceFile={}\n\n", relSourceFilename));

	auto writePass = [&](StrView entryPoint, ShaderStageFlags stageFlags, StrView profile) {
		if (!entryPoint) return;

		String outJsonFilename = Fmt("DX12-{0}-{1}.bin.json.tmp", pass.name, stageFlags);
		outJsonFileList.append(outJsonFilename);

		outStr.append(Fmt("build {}: build_dx12_bin_json ${{SourceFile}} | ${{AxShaderTool}}\n", outJsonFilename, pass.name, stageFlags));
		outStr.append(Fmt("  param_entry_point = {}\n", entryPoint));
		outStr.append(Fmt("  param_profile     = {}\n", profile));
		outStr.append("\n");

		// have to use dxc.exe from MS, because that add validate hash in the bin file
		outStr.append(Fmt("build DX12-{}-{}.bin: build_dx12_bin ${{SourceFile}} | {} \n", pass.name, stageFlags, outJsonFilename));
		outStr.append(Fmt("  param_entry_point = {}\n", entryPoint));
		outStr.append(Fmt("  param_profile     = {}\n", profile));
		outStr.append("\n");
	};

	writePass(pass.vsFunc, ShaderStageFlags::Vertex  , "vs_6_0");
	writePass(pass.psFunc, ShaderStageFlags::Pixel   , "ps_6_0");
	writePass(pass.gsFunc, ShaderStageFlags::Geometry, "gs_6_0");
}
#endif // #if AX_RENDERER_DX12

#if AX_RENDERER_NULL
void AxShaderTool_App::writeNinja_NullPass(IString& outStr,
	IArray<String>& outJsonFileList,
	ShaderPassInfo& pass,
	StrView relSourceFilename
) {
	outStr.append(	"#---- Null ----\n"
					"rule build_null_bin\n"
					"  depfile = $out.d\n"
					"  command = \"$vulkan_sdk/Bin/glslc\" $\n"
					"    -x hlsl $\n"
					"    -fshader-stage=$param_shader_stage $\n"
					"    -fentry-point=$param_entry_point $\n"
					"    -fauto-bind-uniforms $\n"
#if AX_RENDER_BINDLESS
					"    -DAX_RENDER_BINDLESS=1 $\n"
#endif
					"    -Werror $\n" // Treat warnings as errors
					"    -MD -MF \"$out.d\""
					"    -I \"$AxIncludeDir\""
					"    -o \"$out\" \"$in\""
					"\n\n");

#if 0
	outStr.append(	"rule build_null_reflect\n"
					"  command = \"$vulkan_sdk/Bin/spirv-cross\" $\n"
					"    --reflect $\n"
					"    --remove-unused-variables $\n"
					"    --hlsl-auto-binding sampler $\n" // assign register id "Texture2D NAME : register(t ## REG); "
					"    --set-hlsl-vertex-input-semantic 100 POSITION $\n"
					"    --output \"$out\" \"$in\""
					"\n\n");
#endif

	outStr.append(	"rule build_null_bin_json\n"
					"  command = \"${AxShaderTool}\" $\n"
					"    -genReflect_Null $\n"
					"    -file=\"$in\""
					"    -out=\"$out\" $\n"
					"\n\n");

	outStr.append(Fmt("SourceFile={}\n\n", relSourceFilename));

	auto writePass = [&](StrView entryPoint, ShaderStageFlags stageFlags, StrView profile) {
		if (!entryPoint) return;

		outStr.append(Fmt("build NULL-{0}-{1}.bin: build_null_bin ${{SourceFile}} | ${{AxShaderTool}}\n", pass.name, stageFlags));
		outStr.append(Fmt("  param_shader_stage = {}\n", profile));
		outStr.append(Fmt("  param_entry_point  = {}\n", entryPoint));
		outStr.append("\n");

#if 0
		outStr.append(Fmt("build NULL-{0}-{1}.reflect.json.tmp: build_null_reflect NULL-{0}-{1}.bin\n", pass.name, stageFlags));
		outStr.append("\n");
#endif

		String outJsonFilename = Fmt("NULL-{0}-{1}.bin.json.tmp", pass.name, stageFlags);
		outJsonFileList.append(outJsonFilename);

		outStr.append(Fmt("build {}: build_null_bin_json NULL-{}-{}.bin\n", outJsonFilename, pass.name, stageFlags));
		outStr.append("\n");
	};

	writePass(pass.vsFunc, ShaderStageFlags::Vertex  , "vertex"  );
	writePass(pass.psFunc, ShaderStageFlags::Pixel   , "fragment");
	writePass(pass.gsFunc, ShaderStageFlags::Geometry, "geometry");
}
#endif // #if AX_RENDERER_NULL

#if AX_RENDERER_VK
void AxShaderTool_App::writeNinja_VulkanPass(IString& outStr, IArray<String>& outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename) {
	outStr.append(	"#---- Vulkan ----\n"
					"rule build_vk_bin\n"
					"  depfile = $out.d\n"
					"  command = \"$vulkan_sdk/Bin/glslc\" $\n"
					"    -x hlsl $\n"
					"    -fshader-stage=$param_shader_stage $\n"
					"    -fentry-point=$param_entry_point $\n"
					"    -fauto-bind-uniforms $\n"
#if AX_RENDER_BINDLESS
					"    -DAX_RENDER_BINDLESS=1 $\n"
#endif
					"    -Werror $\n" // Treat warnings as errors
					"    -MD -MF \"$out.d\""
					"    -I \"$AxIncludeDir\""
					"    -o \"$out\" \"$in\""
					"\n\n");

#if 0
	outStr.append(	"rule build_vk_reflect\n"
					"  command = \"$vulkan_sdk/Bin/spirv-cross\" $\n"
					"    --reflect $\n"
					"    --remove-unused-variables $\n"
					"    --hlsl-auto-binding sampler $\n" // assign register id "Texture2D NAME : register(t ## REG); "
					"    --set-hlsl-vertex-input-semantic 100 POSITION $\n"
					"    --output \"$out\" \"$in\""
					"\n\n");
#endif

	outStr.append(	"rule build_vk_bin_json\n"
					"  command = \"${AxShaderTool}\" $\n"
					"    -genReflect_VK $\n"
					"    -file=\"$in\""
					"    -out=\"$out\" $\n"
					"\n\n");

	outStr.append(Fmt("SourceFile={}\n\n", relSourceFilename));

	auto writePass = [&](StrView entryPoint, ShaderStageFlags stageFlags, StrView profile) {
		if (!entryPoint) return;

		outStr.append(Fmt("build VK-{0}-{1}.bin: build_vk_bin ${{SourceFile}} | ${{AxShaderTool}}\n", pass.name, stageFlags));
		outStr.append(Fmt("  param_shader_stage = {}\n", profile));
		outStr.append(Fmt("  param_entry_point  = {}\n", entryPoint));
		outStr.append("\n");

#if 0
		outStr.append(Fmt("build VK-{0}-{1}.reflect.json.tmp: build_vk_reflect VK-{0}-{1}.bin\n", pass.name, stageFlags));
		outStr.append("\n");
#endif

		String outJsonFilename = Fmt("VK-{0}-{1}.bin.json.tmp", pass.name, stageFlags);
		outJsonFileList.append(outJsonFilename);

		outStr.append(Fmt("build {}: build_vk_bin_json VK-{}-{}.bin\n", outJsonFilename, pass.name, stageFlags));
		outStr.append("\n");
	};

	writePass(pass.vsFunc, ShaderStageFlags::Vertex  , "vertex"  );
	writePass(pass.psFunc, ShaderStageFlags::Pixel   , "fragment");
	writePass(pass.gsFunc, ShaderStageFlags::Geometry, "geometry");
}
#endif // #if AX_RENDERER_VK

void AxShaderTool_App::showHelp() {
	AX_LOG(	"\n==== AxShaderTool_App Help: ====\n"
			"  AxShaderTool_App -genNinja        -file=<file/folder> -out=<folder>\n"
			"  AxShaderTool_App -genReflect_Null -file=<filename>    -out=<outFilename> \n"
			"  AxShaderTool_App -genReflect_VK   -file=<filename>    -out=<outFilename> \n"
			"  AxShaderTool_App -genReflect_DX12 -file=<filename>    -out=<folder> -profile=<profile_name> -entry=<entry_func_name> \n"
			"  AxShaderTool_App -genResultInfo   -file=<filename>    -out=<outFilename> -api=<api>\n"
			);
}

int AxShaderTool_App::onRun() {
//	AX_LOG("currentDir = {}", FilePath::currentDir());
	auto args = commandArguments();

	for (Int i=1; i<args.size(); i++) {
		auto& a = args[i];

		if (a == "-genNinja"			) { opt.genNinja			= true; continue; }
		if (a == "-genResultInfo"		) { opt.genResultInfo		= true; continue; }
		if (a == "-keepUnusedVariable"	) { opt.keepUnusedVariable	= true; continue; }
		if (a == "-quiet"				) { opt.quiet				= true; continue; }
#if AX_RENDERER_NULL
		if (a == "-genReflect_Null"		) { opt.genReflect_Null		= true; continue; }
#endif
#if AX_RENDERER_VK
		if (a == "-genReflect_VK"		) { opt.genReflect_VK		= true; continue; }
#endif
#if AX_RENDERER_DX12
		if (a == "-genReflect_DX12"		) { opt.genReflect_DX12		= true; continue; }
#endif		

		if (auto v = a.extractFromPrefix("-file=")) {
			opt.file = v;
			continue;
		}

		if (auto v = a.extractFromPrefix("-out=")) {
			FilePath::getUnixPath(opt.out, v);
			continue;
		}

		if (auto v = a.extractFromPrefix("-profile=")) {
			opt.profile = v;
			continue;
		}

		if (auto v = a.extractFromPrefix("-api=")) {
			if (! EnumFn(opt.api).tryParse(v)) {
				throw Error_Undefined(Fmt("'-api={}' unknown api", v));
			}
			continue;
		}

		if (a == "-entry=") { // -entry="" <-- value can be empty
			opt.entry = ""; 
			continue;
		}

		if (auto v = a.extractFromPrefix("-entry=")) {
			opt.entry = v;
			continue;
		}

		if (auto v = a.extractFromPrefix("-I=")) {
			opt.include_dirs.emplaceBack(v);
			continue;
		}

		AX_LOG("Unknown argument {}", a);
		showHelp();
		return -1;
	}

	if (!opt.file) {
		AX_LOG("missing -file=<input file>");
		showHelp(); return -1;
	}

	if (opt.genNinja) {
		genNinja_Shader(opt.out, opt.file);

#if AX_RENDERER_NULL
	} else if (opt.genReflect_Null) {
		GenReflect_VK_EX c;
		c.generate(opt.out, opt.file, RenderApi::Null);
#endif

#if AX_RENDERER_VK
	} else if (opt.genReflect_VK) {
		GenReflect_VK_EX c;
		c.generate(opt.out, opt.file, RenderApi::VK);
#endif

#if AX_RENDERER_DX12		
	} else if (opt.genReflect_DX12) {
		GenReflect_DX12 c;
		c.compile(opt.out, opt.file, opt.profile, opt.entry, opt.include_dirs, opt.keepUnusedVariable);
#endif

	} else if (opt.genResultInfo) {
		GenResultInfo c;
		c.run(opt.out, opt.file, opt.api);

	}else{
		AX_LOG("missing generator");
		showHelp();
		return -1;
	}
	return 0;
}

} // namespace
