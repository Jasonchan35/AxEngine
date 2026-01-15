module;

module AxCppHeaderTool;
import :App;
import :Generator;

namespace ax::AxCppHeaderTool {

CmdOptions::CmdOptions() {
	writeFileOpt.logResult   = true;
	writeFileOpt.logNoChange = false;
}

int App::onRun() {
	String currentDir;
	FilePath::getCurrentDir(currentDir);
	AX_LOG("currentDir={}", currentDir);

	auto args = commandArguments();

	String inputPath;

	for (Int i = 1; i < args.size(); i++) {
		auto& a = args[i];
		if (auto outPath = a.extractFromPrefix("-outPath=")) {
			opt.outPath = FilePath::absPath(outPath);
			
		} else if (auto moduleName = a.extractFromPrefix("-moduleName=")) {
			opt.moduleName = moduleName;
			
		} else if (a.startsWith("-")) {
			AX_LOG("unknown option {}", a);
			return -1;
		} else {
			inputPath = FilePath::absPath(a);
		}
	}

	if (!inputPath) {
		AX_LOG_ERROR("missing inputPath");
	}	
	
	if (!opt.outPath) {
		AX_LOG_ERROR("missing -outPath=<path>");
		return -1;
	}
	
	AX_LOG("outPath={}", opt.outPath);

	// TempString searchFile;
	// searchFile.set(inputPath, "/**/*.h");
	auto searchFile = Fmt("{}/**/*.cppm", inputPath);
	AX_LOG("searchFile={}", searchFile);
	
	String _outGenHeader;
	String _outGenTypes("void registerTypes() {\n"
						"\t""auto* mgr = AxEngine::ObjectManager::s_instance();\n"
						"\t""AX_UNUSED(mgr);");

	Array<String> inputFiles;
	File::glob(searchFile, [&inputFiles](FileEntry & entry) {
		inputFiles.append(entry.fullpath);
	});

	if (opt.moduleName) {
		_outGenHeader.appendFormat("module {};\n", opt.moduleName);
		_outGenHeader.appendFormat("import :Common;\n\n");
	}
	
	inputFiles.sort();

	for (auto& f : inputFiles) {
		Generator g;
		g.gen(opt, f);

		if (!g._typeDB.types.size())
			continue;

		FilePath::getRelPath(searchFile, f, inputPath);
		_outGenHeader.appendFormat("import :{};\n", g._parser.modulePartition());
		_outGenHeader << "\n";
		
		for (auto& t : g._typeDB.types.values()) {
			_outGenTypes.appendFormat("\t""mgr->addType<{:40}>();\n", t.fullname);
		}
	}

	_outGenTypes.append("}\n\n");
	
	_outGenHeader.append(_outGenTypes);
	
	TempString outFilename(opt.outPath, "/GenTypes.gen.cpp");
	File::writeFileIfChanged(outFilename, _outGenHeader, opt.writeFileOpt);
	
	return 0;
}


} //namespace

