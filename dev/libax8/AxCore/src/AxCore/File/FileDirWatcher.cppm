module;


export module AxCore.FileDirWatcher;
export import AxCore.Delegate;
export import AxCore.FilePath;

export namespace ax {
	
#define AX_CORE_FileDirWatcher_Action_ENUM_LIST(E) \
	E(None,) \
	E(Added,) \
	E(RenamedNewName,) \
	E(Removed,) \
	E(RemovedOldName,) \
	E(Modified,) \
//----
AX_ENUM_FLAGS_CLASS(AX_CORE_FileDirWatcher_Action_ENUM_LIST, FileDirWatcher_Action, u8)

AX_SIMPLE_ERROR(Error_FileDirWatcher)

class FileDirWatcher_Result : public NonCopyable {
public:
	using Action = FileDirWatcher_Action;

	void reset() { list.clear(); }

	String path;

	struct Entry {
		String filename;
		Action action = Action::None;
	};
	Array<Entry>	list;
};

class FileDirWatcher : public NonCopyable {
public:
	using Action = FileDirWatcher_Action;

	using Delegate = Delegate_<void (FileDirWatcher_Result& results)>;
	Delegate	dgResults;

	~FileDirWatcher() { destroy(); }

	void create(StrView path, bool watchSubDir = true);
	void destroy();

private:

#if AX_OS_WINDOWS
	static const Int _MaxBufferSize = 256 * 1024;

	HANDLE hDir = INVALID_HANDLE_VALUE;
	Array<Byte>	_buffer;
	OVERLAPPED _overlapped = {};
	static void CALLBACK _s_onChangeNotification(DWORD errorCode, DWORD bytesTransferred, LPOVERLAPPED lpOverlapped);
	void _onChangeNotification(DWORD errorCode, DWORD bytesTransferred);
#endif
	String	_path;
	FileDirWatcher_Result _result;
	bool _watchSubDir = false;
};

} // namespace