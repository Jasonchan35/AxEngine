module;

export module AxFileTool;
export import AxCore; 

export namespace ax {

class AxFileTool : public ConsoleApp {
public:
	void showHelp();
	int onRun() override;
};

} // namespace