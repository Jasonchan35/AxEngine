module;

export module AxEditor.EditorApp;
export import AxNativeUI;

export namespace ax::AxEditor {

class EditorApp : public NativeUIApp {
public:
	virtual int onRun() override {
		return 0;
	}
};

} // namespace
