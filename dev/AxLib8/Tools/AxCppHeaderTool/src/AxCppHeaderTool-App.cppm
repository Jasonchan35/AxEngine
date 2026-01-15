module;

export module AxCppHeaderTool:App;
export import :PCH;

export namespace ax::AxCppHeaderTool {

struct CmdOptions {
	CmdOptions();
	
	String moduleName;
	String outPath;
	File_WriteOpt writeFileOpt;
};	

class App : public ConsoleApp {
public:
	virtual int onRun() override;
	CmdOptions opt {};
};

} //namespace