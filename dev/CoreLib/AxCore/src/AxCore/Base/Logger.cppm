module;
#include "AxCore-pch.h"

export module AxCore.Logger;
export import AxCore.Formatter;

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
	void log(const SrcLoc& loc, Level lv, const FormatString_<Char, Args...> & fmt, const Args&... args) {
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


} // namespace
