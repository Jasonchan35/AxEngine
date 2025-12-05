module;
#include "AxCore-pch.h"
#include "AxCore/Reflection/AxRtti.h"

export module AxCore.AppBase;
export import AxCore.Rtti;
import AxCore.Logger;

export namespace ax {

class AppArgments : public NonCopyable {
public:
	static AppArgments* s_instance();

	AppArgments(int argc, const char* argv[]);
	~AppArgments();

	Span<StrView>	args() const { return _argsView; }
private:
	Array<StrView>	_argsView;
	Array<String>	_argsStr;
};

class AppBase : public RttiObject {
	AX_RTTI_INFO(AppBase, RttiObject)
public:
	static AppBase* s_instance();

	AppBase();
	virtual ~AppBase() override;

	void	create()	{ onCreate(); }
	virtual void	onCreate() {}
	virtual void	willQuit() {}	
	virtual int		onRun() = 0;

	void	setAppName(StrView s);
	StrView	appName();

	StrView currentExecuteFilename() const;
	Span<StrView> commandArguments() const;

	int	_run();

private:
	String	_currentExecFilename;
	String	_appName;
};

template<class T> inline
int App_run(int argc, const char* argv[]) {
	try {
		AppArgments arg(argc, argv);
		T app;
		int ret = app._run();
		return ret;

	} catch (...) {
		AX_LOG_FLUSH();
		throw;
	}
}

} // namespace
