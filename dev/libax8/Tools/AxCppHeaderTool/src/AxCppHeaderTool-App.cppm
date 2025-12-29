module;

export module AxCppHeaderTool:App;
export import :PCH;

export namespace ax::AxCppHeaderTool {

struct CmdOptions {
	String outPath;
	File::WriteFileOpt writeFileOpt;
	
	CmdOptions();
};	

class App : public ConsoleApp {
public:
	virtual int onRun() override;
	CmdOptions opt {};
};

} //namespace