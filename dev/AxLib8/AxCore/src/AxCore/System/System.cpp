module;

module AxCore.System;
import AxCore.Formatter;
import AxCore.ZStrUtil;

namespace ax {

#if 0
#pragma mark ================= Windows ====================
#endif

#if AX_OS_WINDOWS

Int	System::numberOfProcessors() {
	SYSTEM_INFO	info;
	GetSystemInfo( &info );
	return info.dwNumberOfProcessors;
}	

StrView System::platformName() {
	return StrView("windows");
}

#endif // AX_OS_WINDOWS

#if AX_OS_IOS	
#if 0
#pragma mark ================= iOS ====================
#endif

StrView System::platformName() {
	return StrView("iOS");
}

Int	System::numberOfProcessors() { 
	return [[NSProcessInfo processInfo] processorCount];	
}

Int System::availableMemory() {
	vm_statistics_data_t vmStats;
	mach_msg_type_number_t infoCount = HOST_VM_INFO_COUNT;
	kern_return_t kernReturn = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmStats, &infoCount);
	if(kernReturn != KERN_SUCCESS) {
		return -1;
	}
	return vm_page_size * vmStats.free_count;
}

void System::getMachineName(IString &str) {
	UIDevice* dev = [UIDevice currentDevice];
	str.convert( dev.name );
	return 0;
}

double System::uptime() {
	return [[NSProcessInfo processInfo] systemUptime];
}
	
#endif // AX_OS_IOS

#if AX_OS_MACOSX
#if 0
#pragma mark ================= Mac OS X ====================
#endif

StrView System::platformName() {
	return StrView("macosx");
}

Int System::numberOfProcessors() { 
	return [[NSProcessInfo processInfo] processorCount];	
}

void System::getMachineName(IString &str) {
	size_t size;
	//get string size
	sysctlbyname("hw.machine", nullptr, &size, nullptr, 0);
	
	st = str.resize( size, false );			
	if( !st ) return st;
	
	sysctlbyname("hw.machine", str.data(), &size, nullptr, 0);
	return 0;
}

double System::uptime() {
	return [[NSProcessInfo processInfo] systemUptime];	
}

#endif // AX_OS_MACOSX

#if AX_OS_LINUX
#if 0
#pragma mark ================= Linux ====================
#endif

StrView System::platformName() {
	return StrView("linux");
}

Int	System::numberOfProcessors() { 
	return sysconf( _SC_NPROCESSORS_CONF ); 
}

Int System::availableMemory() {
	struct sysinfo a;
	sysinfo( &a );
	return a.freeram;
}

#endif // AX_OS_LINUX

#if 0
#pragma mark ================= Common ====================
#endif

bool System::getEnvVariable(IString& outStr, StrView name) {
#if AX_OS_WINDOWS
	auto nameW = TempStringW::s_utf(name);
	size_t requiredSize = 0;
	_wgetenv_s(&requiredSize, nullptr, 0, nameW.c_str());
	if (requiredSize <= 0) {
		return false;
	}

	TempStringW tmp;
	tmp.resize(ax_safe_cast_from(requiredSize)); 
	errno_t err = _wgetenv_s(&requiredSize, tmp.data(), tmp.size(), nameW.c_str());
	if (err != 0) {
		AX_ASSERT(false);
		return false;
	}

	if (tmp.size() <= 0) {
		AX_ASSERT(false);
		return false;
	}
	
	tmp.popBack();  // -1 for null terminator
	outStr.setUtf(tmp);
	return true;

#else
	auto nameA = TempStringA::s_utf(name);
	char* v = std::getenv(nameA.c_str());
	if (!v) {
		outStr.clear();
		return false;
	}

	outStr.setUtf(ZStrView_c_str(v));
	return true;
#endif
}


Opt<TempString> System::envVariable(StrView name) {
	TempString str;
	if (!getEnvVariable(str, name)) return std::nullopt;
	return str;
}

void System::setEnvVariable(StrView name, StrView value) {
#if AX_OS_WINDOWS
	SetEnvironmentVariableW(TempStringW::s_utf(name).c_str(), TempStringW::s_utf(value).c_str());
#else
	std::setenv(TempString(name).c_str(), TempString(value).c_str());
#endif
}

} // namespace ax
