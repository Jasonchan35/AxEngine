module;

export module AxHeaderTool.App;
export import AxHeaderTool.PCH;

export namespace ax::AxHeaderTool {

class App : public ConsoleApp {
public:
	virtual int onRun() override;
	
	struct CmdOptions {
		String outPath;
	};	
	
	CmdOptions opt {};
};

} //namespace