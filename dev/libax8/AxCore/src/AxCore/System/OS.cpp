module;

// #include "OS.h"

module AxCore.OS;
import AxCore.Formatter;
import AxCore.ZStrUtil;

namespace ax {

ZStrView OS::getenv(ZStrView name) {
	return ZStrView_c_str(std::getenv(name.c_str()));
}

void OS::setenv(StrView name, StrView value) {
#if AX_OS_WINDOWS
	SetEnvironmentVariableW(TempStringW::s_utf(name).c_str(), TempStringW::s_utf(value).c_str());
#else
	std::setenv(TempString(name).c_str(), TempString(value).c_str());
#endif
}


#if 0
#pragma mark ================= Windows ====================
#endif

#if AX_OS_WINDOWS

Int	OS::numberOfProcessors() {
	SYSTEM_INFO	info;
	GetSystemInfo( &info );
	return info.dwNumberOfProcessors;
}	

StrView OS::platformName() {
	return StrView("windows");
}

#endif // AX_OS_WINDOWS

#if AX_OS_IOS	
#if 0
#pragma mark ================= iOS ====================
#endif

StrView OS::platformName() {
	return StrView("iOS");
}

Int	OS::numberOfProcessors() { 
	return [[NSProcessInfo processInfo] processorCount];	
}

Int OS::availableMemory() {
	vm_statistics_data_t vmStats;
	mach_msg_type_number_t infoCount = HOST_VM_INFO_COUNT;
	kern_return_t kernReturn = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmStats, &infoCount);
	if(kernReturn != KERN_SUCCESS) {
		return -1;
	}
	return vm_page_size * vmStats.free_count;
}

void OS::getMachineName(IString &str) {
	UIDevice* dev = [UIDevice currentDevice];
	str.convert( dev.name );
	return 0;
}

double OS::uptime() {
	return [[NSProcessInfo processInfo] systemUptime];
}
	
#endif // AX_OS_IOS

#if AX_OS_MACOSX
#if 0
#pragma mark ================= Mac OS X ====================
#endif

StrView OS::platformName() {
	return StrView("macosx");
}

Int OS::numberOfProcessors() { 
	return [[NSProcessInfo processInfo] processorCount];	
}

void OS::getMachineName(IString &str) {
	size_t size;
	//get string size
	sysctlbyname("hw.machine", nullptr, &size, nullptr, 0);
	
	st = str.resize( size, false );			
	if( !st ) return st;
	
	sysctlbyname("hw.machine", str.data(), &size, nullptr, 0);
	return 0;
}

double OS::uptime() {
	return [[NSProcessInfo processInfo] systemUptime];	
}

#endif // AX_OS_MACOSX

#if AX_OS_LINUX
#if 0
#pragma mark ================= Linux ====================
#endif

StrView OS::platformName() {
	return StrView("linux");
}

Int	OS::numberOfProcessors() { 
	return sysconf( _SC_NPROCESSORS_CONF ); 
}

Int OS::availableMemory() {
	struct sysinfo a;
	sysinfo( &a );
	return a.freeram;
}

#endif // AX_OS_LINUX

} // namespace ax
