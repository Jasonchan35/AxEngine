module;


export module AxCore.Logger;
export import AxCore.Formatter;
export import AxCore.Array;

export namespace ax {

enum class LogLevel {
	None,
	Info,
	Warning,
	Error,
};

class LogIOutput {
public:
	using Level = LogLevel;

	virtual ~LogIOutput() {}
	virtual void write(Level lv, ZStrViewW s) = 0;
	virtual void flush() = 0;
};

class StdOutput : public NonCopyable {
public:
	static void write(StrViewW s) { std::wcout.write(s.data(), s.size()); }
	static void write(StrViewA s) { write(TempStringW::s_utf(s)); }
};

class StdError : public NonCopyable {
public:
	static void write(StrViewW s) { std::wcerr.write(s.data(), s.size()); }
	static void write(StrViewA s) { write(TempStringW::s_utf(s)); }
};

class LogStdOutput : public LogIOutput {
public:
	static LogStdOutput* s_get();
	virtual void write(Level lv, ZStrViewW s) override;
	virtual void flush() override;
};

class Logger {
public:
	using Level = LogLevel;
	static bool s_showSourceLocation;

	template<class... Args>
	void log(const SrcLoc& loc, Level lv, const FormatString<Args...> & fmt, const Args&... args) {
		if (!_output) return;
		TempString tmp;
		FmtTo(tmp, fmt, args...);
		write(lv, tmp);
	}

	void write(Level lv, StrView msg);

	void setOutput(LogIOutput* p) { _output = p; }

	void flush() { _output->flush(); }

	static Logger*	s_get();

private:
	Logger() {
		setOutput(&_stdOutput);
	}
	LogStdOutput	_stdOutput;
	LogIOutput* _output;
};

#if AX_OS_WINDOWS

class AX_Win32_Error {
	using This = AX_Win32_Error;
public:
	AX_Win32_Error() = default;
	AX_Win32_Error(DWORD code) : _code(code) {};
	static This s_lastError() { return ::GetLastError(); }

	explicit operator bool() const { return _code == ERROR_SUCCESS; }
	
	template<class CH> void onFormat(Format_<CH>& fmt) const {
		LPWSTR tmp = nullptr;
		auto scope = ScopeLambda([&tmp]() { LocalFree(tmp); });
		
		FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					   nullptr, _code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   reinterpret_cast<LPTSTR>(&tmp), 0, nullptr);
		fmt << Fmt("{}: {}", _code, StrView_c_str(tmp));
	}

private:
	DWORD	_code = ERROR_SUCCESS;
};

class AX_Win32_HRESULT {
public:
	AX_Win32_HRESULT() = default;
	AX_Win32_HRESULT(HRESULT hr) : _hr(hr) {};

	explicit operator bool() const { return _hr >= 0; }

	template<class CH> void onFormat(Format_<CH>& fmt) const {
		_com_error err(_hr, nullptr);
		auto* tmp = err.ErrorMessage();
		fmt << Fmt("{:08X}: {}", _hr, StrView_c_str(tmp));
	}

private:
	HRESULT	_hr = S_OK;
};

#endif

} // namespace
