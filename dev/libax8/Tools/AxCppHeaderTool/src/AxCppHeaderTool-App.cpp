module;

module AxCppHeaderTool;
import :App;
import :Generator;

namespace ax::AxCppHeaderTool {

CmdOptions::CmdOptions() {
	writeFileOpt.logResult   = true;
	writeFileOpt.logNoChange = true;
}

int App::onRun() {
	String currentDir;
	FilePath::getCurrentDir(currentDir);
	AX_LOG("currentDir={}", currentDir);

	auto args = commandArguments();

	String inputPath;

	for (Int i = 1; i < args.size(); i++) {
		auto& a = args[i];
		if (auto v = a.extractFromPrefix("-outPath=")) {
			opt.outPath = FilePath::absPath(v);
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

	String	_outGenTypeHeaders("#pragma once\n\n");
	String	_outGenTypes("template<class HANDLER> inline\n"
							 "static void generated_node_types(HANDLER& handler) {\n");

	Array<String> inputFiles;
	File::glob(searchFile, [&inputFiles](FileEntry & entry) {
		inputFiles.append(entry.fullpath);
	});
	
	inputFiles.sort();

	for (auto& f : inputFiles) {
		Generator g;
		g.gen(opt, f);

		if (!g._typeDB.types.size())
			continue;

		FilePath::getRelPath(searchFile, f, inputPath);
		_outGenTypeHeaders.appendFormat("#include \"{}\"\n", searchFile);

		for (auto& t : g._typeDB.types.values()) {
			_outGenTypes.appendFormat("\t""handler.template addType< {:40} >();\n", t.fullname);
		}
	}

	_outGenTypes.append("}\n\n");

	{
		TempString txt(_outGenTypeHeaders, "\n\n", _outGenTypes);
		TempString outFilename(opt.outPath, "/NodeGenTypes._impl.h");
		File::writeFileIfChanged(outFilename, txt, opt.writeFileOpt);
	}

	return 0;
} // onRun


} //namespace

