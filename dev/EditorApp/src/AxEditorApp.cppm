module;

export module AxEditor.EditorApp;
export import AxCore.ConsoleApp;

export namespace ax::AxEditor {

class EditorApp : public ConsoleApp {
public:
	virtual int onRun() override {
		return 0;
	}
};

} // namespace
