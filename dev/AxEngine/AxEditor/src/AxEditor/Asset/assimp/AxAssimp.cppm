module;

export module AxEditor:AxAssimp;
export import :Common;

export namespace AxEditor {

class AxAssimp : public NonCopyable {
public:
	void openFile(StrView filename);
};

} // namespace
