module;

export module AxEditor:AxOpenUsd;
export import :Common;

export namespace AxEditor {

class AxOpenUsd : public NonCopyable {
public:
	AxOpenUsd();
	
	void openFile(StrView filename);
};

} // namespace