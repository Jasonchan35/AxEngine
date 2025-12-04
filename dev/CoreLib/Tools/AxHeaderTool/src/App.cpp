module;

#include "AxHeaderTool-pch.h"

module AxHeaderTool.App;
import AxHeaderTool.Generator;

namespace ax::AxHeaderTool {

int App::onRun() {
	String currentDir;
	axPath::getCurrentDir(currentDir);
	AX_DUMP_VAR(currentDir);


	auto args = commandArguments();

	String inputPath;

	for (Int i = 1; i < args.size(); i++) {
		auto& a = args[i];
		if (a.startsWith("-")) {
			AX_LOG("unknown option {?}", a);
			return -1;
		} else {
			inputPath = a;
		}
	}

	Array<String> inputFiles;
	TempString tmp;

	tmp.set(inputPath, "/**/*.h");
	AX_DUMP_VAR(tmp);

	String	_outGenTypeHeaders("#pragma once\n\n");
	String	_outGenTypes("template<class HANDLER> inline\n"
							 "static void generated_node_types(HANDLER& handler) {\n");

	axPath::glob(inputFiles, tmp, false, true, false);
	inputFiles.sort();

	for (auto& f : inputFiles) {
		Generator g;
		g.gen(f);

		if (!g._typeDB.types.size())
			continue;

		axPath::getRelative(tmp, f, inputPath);
		_outGenTypeHeaders.appendFormat("#include \"{}\"\n", tmp);

		for (auto& t : g._typeDB.types.values()) {
			_outGenTypes.appendFormat("\t""handler.template addType< {,40} >();\n", t.fullname);
		}
	}

	_outGenTypes.append("}\n\n");

	{
		TempString txt(_outGenTypeHeaders, "\n\n", _outGenTypes);
		TempString outFilename(inputPath, "/NodeGenTypes._impl.h");
		axFile::writeFileIfChanged(outFilename, txt, true, false);
	}

	return 0;
} // onRun


} //namespace

AX_APP_MAIN(ax::AxHeaderTool::App);
