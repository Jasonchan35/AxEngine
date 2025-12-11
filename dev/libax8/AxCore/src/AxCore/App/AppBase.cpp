module;



module AxCore.AppBase;
import AxCore.FilePath;
import AxCore.Logger;

namespace ax {

static AppBase* AppBase_instance;

AppBase* AppBase::s_instance() {
	return AppBase_instance;
}

AppBase::AppBase() {
	AX_ASSERT(AppBase_instance == nullptr);
	AppBase_instance = this;

#if AX_OS_WINDOWS
	const int tmpSize = 2048;
	wchar_t	tmp[tmpSize];

	auto len = GetModuleFileNameW(nullptr, tmp, tmpSize);
	_currentExecFilename.setUtf(StrViewW(tmp, len));
#else
	auto args = commandArguments();
	if (args.size()) {
		_currentExecFilename =  args[0];
	}
#endif

}

AppBase::~AppBase() {
	AX_ASSERT(AppBase_instance == this);
	AppBase_instance = nullptr;
}

void AppBase::setAppName(StrView s) {
	_appName = s;
}

StrView AppBase::appName() {
	if (!_appName) {
		auto s = currentExecuteFilename();
		_appName = FilePath::basename(s, false);
	}
	return _appName;
}

StrView AppBase::currentExecuteFilename() const {
	return _currentExecFilename;
}

Span<StrView> AppBase::commandArguments() const { 
	auto* p = AppArgments::s_instance();
	return p ? p->args() : Span<StrView>();
}

int	AppBase::_run() {
	const bool enableAssert = true;

	try{
		return onRun();
	}catch(Error& err) {
		AX_LOG_ERROR("{}", err);
		AX_LOG_FLUSH();
		if (enableAssert) {
			AX_ASSERT(false);
		}
		return -9999;
	}catch(std::exception& e) {
		AX_LOG_ERROR("std::exception {}", e);
		AX_LOG_FLUSH();
		if (enableAssert) {
			AX_ASSERT(false);
		}
		return -9999;
	}catch(...) {
		AX_LOG_ERROR("unknown exception");
		AX_LOG_FLUSH();
		if (enableAssert) {
			AX_ASSERT(false);
		}
		return -9999;
	}
}

//--------------

static AppArgments* AppArgments_instance;

AppArgments* AppArgments::s_instance() {
	return AppArgments_instance;
}

AppArgments::AppArgments(int argc, const char* argv[]) {
	AX_ASSERT(!AppArgments_instance);
	AppArgments_instance = this;

	#if AX_OS_WINDOWS
		int count = 0;
		auto* a = CommandLineToArgvW(GetCommandLineW(), &count);
		Span<wchar_t*> args(a, count);

		_argsStr.resize(count);
		for (Int i=0; i<argc; i++) {
			_argsStr[i].setUtf(StrView_c_str(args[i]));
		}
	#elif AX_OS_MACOSX
		NSProcessInfo* info = [NSProcessInfo processInfo];
		NSArray* a = [info arguments];
		_argsStr.resize((axInt)a.count);
		for (size_t i=0; i<a.count; i++) {
			auto s = [[a objectAtIndex : i] UTF8String];
			_argsStr[Int(i)].setUtf(StrView_c_str(s));
		}
	#else
		_argsStr.resize(argc);
		for (Int i=0; i<argc; i++) {
			_argsStr[i].setUtf(StrView_c_str(argv[i]));
		}
	#endif

	_argsView.ensureCapacity(_argsStr.size());
	for (auto& e : _argsStr) {
		_argsView.append(e.view());
	}
}

AppArgments::~AppArgments() {
	AX_ASSERT(AppArgments_instance == this);
	AppArgments_instance = nullptr;
}

} // namespace ax
