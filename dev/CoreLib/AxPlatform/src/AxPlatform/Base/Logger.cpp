module AxPlatform.Logger;

namespace  ax {

Logger* Logger::s_get() {
	static Logger s;
	return &s;
}

LogStdOutput* LogStdOutput::s_get() {
	static LogStdOutput s;
	return &s;
}

void LogStdOutput::flush() { std::flush(std::wcout); }

void Logger::write(Level lv, StrView msg) {
	TempStringW tmp;

	// \x1b = ANSI escape sequences
	switch (lv) {		
		case Level::Warning: tmp.append(L"\x1b[33mWarning: "); break; // yellow
		case Level::Error:   tmp.append(L"\x1b[31mError: ");   break; // red
		default: break;
	}

	tmp.appendUtf(msg);
	tmp.append(L"\n");
	tmp.append(L"\x1b[0m"); // reset color

	_output->write(lv, tmp);
}

void LogStdOutput::write(Level lv, ZStrViewW s) {
	if (lv == Level::Error) {
		StdError::write(s);
	} else {
		StdOutput::write(s);
	}

#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_INFO, "libax", msg);

#elif AX_OS_WINDOWS
	if (IsDebuggerPresent()) {
		OutputDebugString(s.c_str());
	}
#else
	//
#endif
}

} // namespace
