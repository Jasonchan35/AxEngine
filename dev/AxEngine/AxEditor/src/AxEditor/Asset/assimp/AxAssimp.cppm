module;

export module AxEditor:AxAssimp;
export import :Common;

export namespace AxEditor {

class AxAssimp : public NonCopyable {
public:
	AxAssimp();
	
	void openFile(StrView filename);
};

} // namespace
