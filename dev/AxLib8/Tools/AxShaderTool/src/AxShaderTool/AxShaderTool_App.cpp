module AxShaderTool;
import :App;
import :GenReflect_Null;
import :GenReflect_Dx12;
import :GenReflect_Vk;
import :ShaderInfoParser;
import :GenResultInfo;

namespace ax /*::AxRender*/ {

CmdOptions::CmdOptions() {
	writeFileOpt.createDir = true;
	writeFileOpt.logResult = true;
}

CmdOptions& CmdOptions::s_instance() {
	static CmdOptions s;
	return s;
}

AxShaderTool_App::AxShaderTool_App() {
//	Error::s_setEnableAssertion(true);
}

void AxShaderTool_App::genNinja_ShadersInFolder(StrView outDir, StrView filename) {
	Array<String>	outFiles;
	File::glob(Fmt("{}/**/*.axShader", filename),
		[&](auto& entry) {
			auto relFile = FilePath::relPath(entry.fullpath, filename);
			auto relOutDir = Fmt("{}/{}", outDir, relFile);
			outFiles.emplaceBack(relFile);
			genNinja_Shader(relOutDir, entry.fullpath);
		});

	File::glob(Fmt("{}/**/*.axComputeShader", filename),
		[&](auto& entry) {
			auto relFile = FilePath::relPath(entry.fullpath, filename);
			auto relOutDir = Fmt("{}/{}", outDir, relFile);
			outFiles.emplaceBack(relFile);
			genNinja_Shader(relOutDir, entry.fullpath);
		});

	genNinja_Shaders(outDir, outFiles);
}

void AxShaderTool_App::genNinja_Shaders(StrView outDir, const Array<String>& files) {
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

	File::writeFileIfChanged(Fmt("{}/build.ninja", outDir), outStr, opt.writeFileOpt);
}

void AxShaderTool_App::genNinja_Shader(StrView outDir, StrView filename) {
	if (!outDir) {
		AX_LOG("error: please specify outDir");
		throw Error_Undefined();
	}

	if (FileDir::exists(filename)) { // if the input is a directory
		genNinja_ShadersInFolder(outDir, filename);
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
	parser.readFile(outDir, filename);

	opt.keepUnusedVariable = parser.info.isGlobalCommonShader;

	outStr.append(Fmt("SourceFile={}\n\n", filename));

	auto func = [&](RenderAPI api) {
		outStr.append(Fmt("build {}/shaderResult.json: build_api_shader {} | ${{SourceFile}} ${{AxShaderTool}} \n", api, api));
		genNinja_Shader_API(api, parser.info, outDir, filename);
	};

#if AX_RENDERER_NULL
	func(RenderAPI::Null);
#endif
#if AX_RENDERER_VK	
	func(RenderAPI::Vk);
#endif
#if AX_RENDERER_DX12
	func(RenderAPI::Dx12);
#endif

	auto outFilename = Fmt("{}/build.ninja", outDir);
	File::writeFileIfChanged(outFilename, outStr, opt.writeFileOpt);
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

	outStr.append("\n#-------- Common Header -----------------\n");
	
	writeEnvVar("ninja");
	writeEnvVar("vulkan_sdk");
	writeEnvVar("windows_sdk_bin");
	writeEnvVar("AxEngine_bin");
	writeEnvVar("AxIncludeDir");
	writeEnvVar("AxShaderTool");
	outStr.append("\n\n");
}

void AxShaderTool_App::genNinja_Shader_API(RenderAPI api, ShaderDeclareInfo& info, StrView outDir, StrView filename) {	
	auto apiOutDir = Fmt("{}/{}", outDir, api);

	auto absSourceFilename = FilePath::absPath(filename);

	String outStr;
	writeNinja_Header(outStr);

	Array<String, 64>	depFileList;
//---
	outStr.append("\n#---------- API -----------------\n");
	for (auto& pass : info.passes) {
		switch (api) {
#if AX_RENDERER_NULL
			case RenderAPI::Null:   GenNinja_Null().writeNinjaPass(outStr, depFileList, pass, absSourceFilename); break;
#endif
#if AX_RENDERER_VK
			case RenderAPI::Vk:		GenNinja_Vk().writeNinjaPass(outStr, depFileList, pass, absSourceFilename); break;
#endif
#if AX_RENDERER_DX12
			case RenderAPI::Dx12:	GenNinja_Dx12().writeNinjaPass(outStr, depFileList, pass, absSourceFilename); break;
#endif
			default: break;
		}
	}

	outStr.append(	"\n#--------- shaderResult --------------------------\n"
					"rule build_shaderResult_json\n"
					"  command = ${AxShaderTool} $\n"
					"    -genResultInfo $\n"
					"    -api=$param_api $\n"
					"    -file=\"$in\" $\n"
					"    -out=\"$out\" $\n"
					"\n\n");

	outStr.append("build shaderResult.json: build_shaderResult_json ../info.json | ${AxShaderTool} ");
	for (auto& f : depFileList) {
		outStr.append(Fmt(" {}", f));
	}
	outStr.append("\n");
	outStr.append(Fmt("  param_api  = {}\n ", api      ));
	outStr.append("\n\n");

	auto outFilename = Fmt("{}/build.ninja", apiOutDir);
	File::writeFileIfChanged(outFilename, outStr, opt.writeFileOpt);
}

void AxShaderTool_App::showHelp() {
	AX_LOG(	"\n==== AxShaderTool_App Help: ====\n"
			"  AxShaderTool_App -genNinja        -file=<file/folder> -out=<folder>\n"
			"  AxShaderTool_App -genReflect_Null -file=<filename>    -out=<outFilename> \n"
			"  AxShaderTool_App -genReflect_Vk   -file=<filename>    -out=<outFilename> \n"
			"  AxShaderTool_App -genReflect_Dx12 -file=<filename>    -out=<folder> -profile=<profile_name> -entry=<entry_func_name> \n"
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
#if AX_RENDERER_NULL
		if (a == "-genReflect_Null"		) { opt.genReflect_Null		= true; continue; }
#endif
#if AX_RENDERER_VK
		if (a == "-genReflect_Vk"		) { opt.genReflect_Vk		= true; continue; }
#endif
#if AX_RENDERER_DX12
		if (a == "-genReflect_Dx12"		) { opt.genReflect_Dx12		= true; continue; }
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
			if (!v.tryParse(opt.api)) {
				throw Error_Undefined(Fmt("'-api={}' unknown api", v));
			}
			continue;
		}

		if (a == "-quiet") { opt.quiet= true; continue; }
		if (auto v = a.extractFromPrefix("-quiet=")) {
			int tmp = 0;
			if (!v.tryParse(tmp)) {
				throw Error_Undefined(Fmt("'-quiet={}' unknown value", v));
			}
			opt.quiet = tmp;
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

	if (!opt.file && !opt.genReflect_Null) {
		AX_LOG("missing -file=<input file>");
		showHelp(); return -1;
	}

	if (opt.genNinja) {
		genNinja_Shader(opt.out, opt.file);

#if AX_RENDERER_NULL
	} else if (opt.genReflect_Null) {
		GenReflect_Null c;
		c.generate(opt.out, opt.file, RenderAPI::Null);
#endif

#if AX_RENDERER_VK
	} else if (opt.genReflect_Vk) {
		GenReflect_Vk c;
		c.generate(opt.out, opt.file, RenderAPI::Vk);
#endif

#if AX_RENDERER_DX12		
	} else if (opt.genReflect_Dx12) {
		GenReflect_Dx12 c;
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
