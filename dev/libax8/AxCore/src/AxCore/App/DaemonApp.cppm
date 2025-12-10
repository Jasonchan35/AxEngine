module;


export module AxCore.DaemonApp;
export import AxCore.ConsoleApp;
export import AxCore.ThreadUtil;

export namespace ax {

class DaemonApp : public ConsoleApp {
	AX_RTTI_INFO(DaemonApp, ConsoleApp)
public:
	static DaemonApp* s_instance();

	DaemonApp();
	virtual ~DaemonApp() override;

	bool quitApp() const { return _quitApp; }

protected:
	virtual void onDaemonRun();
	virtual void onCreate() override {}
	virtual void onStart() {}
	virtual void onStop() {}
	virtual void onTick() { Thread::sleep(Milliseconds(100)); }

	void setAppName(StrView name) { _name = name; }

private:
#if AX_OS_WINDOWS
	static	BOOL	WINAPI s_winConsoleCtrlHandler(DWORD dwCtrlType);
	static	void	WINAPI s_winServiceMain(DWORD dwNumServicesArgs, LPWSTR *lpServiceArgVectors);
	static	DWORD	WINAPI s_winServiceStatusHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

	void _winServiceMain(DWORD dwNumServicesArgs, LPWSTR *lpServiceArgVectors);
	void _setWinServiceStatus(DWORD status);

	void installWinService(StrView params, bool autoStart, StrView description = StrView());
	void removeWinService();

	SERVICE_STATUS_HANDLE _winServiceStatusHandle = nullptr;
#else
	static 	void s_singalHandler(int sig);
	bool _setupDaemon();

#endif

	virtual int onRun() final;
	void _daemonRun();

	String	_name;
	bool	_quitApp = false;
};

} // namespace ax