module;

export module AxEditor:AxOpenUSD;
export import :Common;

export namespace AxEditor {

class AxOpenUSD : public NonCopyable {
public:
	AxOpenUSD();
	
	void openFile(StrView filename);
};

} // namespace