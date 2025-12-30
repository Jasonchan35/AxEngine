module;

module AxCore.DaemonApp;
import AxCore.Logger;

namespace ax {

static DaemonApp* Daemon_instance = nullptr;

DaemonApp* DaemonApp::s_instance() {
	return Daemon_instance;
}

DaemonApp::DaemonApp() {
	_appName = "DaemonApp";

	AX_ASSERT(Daemon_instance == nullptr);
	Daemon_instance = this;
}

DaemonApp::~DaemonApp() {
	if (Daemon_instance == this) {
		Daemon_instance = nullptr;
	}
}

void DaemonApp::onDaemonRun() {
	while (!_quitApp) {
		onTick();
	}
}

void DaemonApp::_daemonRun() {
	create();
	onStart();
	onDaemonRun();
	onStop();
}

#if AX_OS_WINDOWS

int DaemonApp::onRun() {
	constexpr bool console = true;

	if (console) {
		::SetConsoleCtrlHandler(s_winConsoleCtrlHandler, true);
		_daemonRun();
	} else {
		auto name = TempStringW::s_utf(_appName);

		FixedArray<SERVICE_TABLE_ENTRY, 2> entries;
		entries[0].lpServiceName = name.data();
		entries[0].lpServiceProc = s_winServiceMain;
		entries[1].lpServiceName = nullptr;
		entries[1].lpServiceProc = nullptr;

		BOOL b;
		b = StartServiceCtrlDispatcher(entries.data());
		if( !b ) { 
			AX_LOG_WIN32_LAST_ERROR("run");
			throw Error_Undefined();
		}
	}

	return 0;
}

void DaemonApp::s_winServiceMain(DWORD dwNumServicesArgs, LPWSTR *lpServiceArgVectors) {
	s_instance()->_winServiceMain(dwNumServicesArgs, lpServiceArgVectors);
}

void DaemonApp::_winServiceMain(DWORD dwNumServicesArgs, LPWSTR *lpServiceArgVectors) {
	auto name = TempStringW::s_utf(_appName);
	_winServiceStatusHandle = ::RegisterServiceCtrlHandlerEx(name.c_str(), s_winServiceStatusHandler, nullptr);
	if (!_winServiceStatusHandle) {
		_setWinServiceStatus(SERVICE_STOPPED);
		return;
	}

	_setWinServiceStatus(SERVICE_RUNNING);
	onRun();

	_setWinServiceStatus(SERVICE_STOPPED);
}

BOOL WINAPI DaemonApp::s_winConsoleCtrlHandler(DWORD dwCtrlType) noexcept {
	switch (dwCtrlType) {
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT: {
			s_instance()->_quitApp = true;
			return true;
		} break;
	}
	return false;
}

DWORD WINAPI DaemonApp::s_winServiceStatusHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) noexcept {
	switch (dwControl) {
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			s_instance()->_setWinServiceStatus(SERVICE_STOP_PENDING);
			break;
	}
	return 0;
}

void DaemonApp::_setWinServiceStatus(DWORD status) {
	SERVICE_STATUS ss;
	ss.dwServiceType              = SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState             = status;
	ss.dwControlsAccepted	      = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;
	ss.dwWin32ExitCode            = NOERROR;
	ss.dwServiceSpecificExitCode  = NOERROR;
	ss.dwCheckPoint               = 0;
	ss.dwWaitHint                 = 3000;
	if (!::SetServiceStatus(_winServiceStatusHandle, &ss) ) {
		AX_LOG_WIN32_LAST_ERROR("_setWinServiceStatus");
	}
}

void DaemonApp::installWinService(StrView params, bool autoStart, StrView description) {
	wchar_t   moduleFilename[MAX_PATH];

	if (::GetModuleFileName(nullptr, moduleFilename, MAX_PATH) == MAX_PATH) {
		AX_LOG_WIN32_LAST_ERROR("");
		throw Error_Undefined();
	}

	SC_HANDLE mgr = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
	if (!mgr) {
		AX_LOG_WIN32_LAST_ERROR("");
		throw Error_Undefined();
	}

	auto cmd = Fmt(L"\"{} {}\"", moduleFilename, params);
	auto name = TempStringW::s_utf(_appName);

	SC_HANDLE svc = CreateService(mgr,
	                              name.c_str(),
	                              name.c_str(),
	                              SERVICE_ALL_ACCESS,
	                              SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
	                              autoStart ? SERVICE_AUTO_START : SERVICE_DEMAND_START,
	                              SERVICE_ERROR_NORMAL,
	                              cmd.c_str(),
	                              nullptr,
	                              nullptr,
	                              nullptr,
	                              nullptr,
	                              nullptr);
	if (!svc) {
		AX_LOG_WIN32_LAST_ERROR("");
		throw Error_Undefined();
	}

	// set service description

	auto descriptionStr = TempStringW::s_utf(description);

	SERVICE_DESCRIPTION desc;
	desc.lpDescription = descriptionStr.data();

	if (!ChangeServiceConfig2(svc, SERVICE_CONFIG_DESCRIPTION, reinterpret_cast<void*>(&desc))) {
		AX_LOG_WIN32_LAST_ERROR("");
		throw Error_Undefined();
	}

	// set service failure restart
	SC_ACTION action[] = {
		{SC_ACTION_RESTART, 60 * 1000},  // 1 min = 60 * 1000
	};

	SERVICE_FAILURE_ACTIONS sfa;
	sfa.cActions	  = sizeof(action) / sizeof(SC_ACTION);
	sfa.lpsaActions	  = action;
	sfa.lpCommand	  = nullptr;
	sfa.lpRebootMsg	  = nullptr;
	sfa.dwResetPeriod = 60 * 60; // 60*60 = rest failure count after 1 hr

	if (!ChangeServiceConfig2(svc, SERVICE_CONFIG_FAILURE_ACTIONS, &sfa)) {
		AX_LOG_WIN32_LAST_ERROR("");
		throw Error_Undefined();
	}

	AX_LOG("Service [{}] installed autoStart={} params=[{}]", name, autoStart, params);
}

void DaemonApp::removeWinService() {

	SC_HANDLE mgr = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
	if (!mgr) {
		AX_LOG_WIN32_LAST_ERROR("");
		throw Error_Undefined();
	}

	auto      name = TempStringW::s_utf(_appName);
	SC_HANDLE svc  = OpenService(mgr, name.c_str(), SERVICE_ALL_ACCESS);
	if (!svc) {
		AX_LOG_WIN32_LAST_ERROR("");
		throw Error_Undefined();
	}

	BOOL b;
	b = DeleteService(svc);
	if (!b) {
		AX_LOG_WIN32_LAST_ERROR("");
		throw Error_Undefined();
	}

	AX_LOG("Service [{}] removed", _appName);
}

#else // AX_OS_WINDOWS

void DaemonApp::s_signalHandler(int sig) {
	switch (sig) {
		case SIGINT:
		case SIGTERM: {
			s_instance()->_quitApp = true;
		}break;
	}
}

bool DaemonApp::_setupDaemon() {
	umask(0);

	// become session leader to lose TTY control
	pid_t pid = fork();
	if (pid < 0) {
		throw Error_Undefined();
	} else if (pid != 0) {
		return false; // parent process
	}
	setsid();

//--------------

	pid = fork();
	if (pid < 0) {
		throw Error_Undefined();
	} else if (pid != 0) {
		return false; // parent process
	}

	{ // redirect stdin, out, err to /dev/null
		int null_fd = open("/dev/null", O_RDWR);
		if (null_fd < 0)
			throw Error_Undefined();

		dup2(null_fd, 0);
		dup2(null_fd, 1);
		dup2(null_fd, 2);
	}

	return true;
}

int DaemonApp::onRun() {
	bool console = true;

	signal(SIGTERM, s_singalHandler); // kill process
	signal(SIGINT,  s_singalHandler); // Ctrl + C

	if (!console) {
		if (!_setupDaemon())
			return 0;
	}

	_daemonRun();
	return 0;
}

#endif

} // namespace ax