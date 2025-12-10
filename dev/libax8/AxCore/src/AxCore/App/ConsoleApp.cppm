module;


export module AxCore.ConsoleApp;
export import AxCore.AppBase;

export namespace ax {

class ConsoleApp : public AppBase {
	AX_RTTI_INFO(ConsoleApp, AppBase)
public:
	static void setConsoleTitle(StrView title) {
#if AX_OS_WINDOWS
		::SetConsoleTitle(TempStringW::s_utf(title).c_str());
#else
#endif
	}
};



} // namespace
