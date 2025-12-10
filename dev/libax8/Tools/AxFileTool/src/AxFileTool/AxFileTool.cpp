module AxFileTool;

namespace ax {

void AxFileTool::showHelp() {
	AX_LOG(	"==== AxFileTool Help: ===="
			"  AxFileTool -touch <filename>\n"
			"  AxFileTool -dos2unix <filename>\n"
			"\n");	
}

int AxFileTool::onRun() {
	auto args = commandArguments();

	for (Int i=1; i<args.size(); i++) {
		auto& a = args[i];
		if (a == "-touch") {
			i++;
			if (i >= args.size()) {
				throw Error_Undefined();
			}

			File::touch(args[i]);
			continue;
		}

		if (a == "-platform_os") {
			i++;
			auto name = OS::platformName();
			StdOutput::write(name);
			StdOutput::write("\n");
			continue;
		}

		if (a == "-dos2unix") {
			i++;
			if (i >= args.size()) {
				throw Error_Undefined();
			}
			TempStringA text;
			File::readUtf8(args[i], text);
			if (text.replaceAll("\r\n", "\n") > 0) {
				File::writeUtf8(args[i], text);
			}
			continue;
		}

		AX_LOG("Unknown argument {}", a);
		showHelp();
		return -1;
	}

	return 0;
}

} // namespace