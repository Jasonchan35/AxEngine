module;



module AxCore.FileDirWatcher;
import AxCore.Logger;

// #include "FileDirWatcher.h"

namespace ax {

#if 0
#pragma mark --------- AX_OS_WINDOWS ---------
#endif
#if AX_OS_WINDOWS

void CALLBACK FileDirWatcher::_s_onChangeNotification (
	DWORD errorCode, DWORD bytesTransferred, LPOVERLAPPED lpOverlapped) noexcept 
{
	auto* p = static_cast<FileDirWatcher*>(lpOverlapped->hEvent);
	p->_onChangeNotification(errorCode, bytesTransferred);
}

void FileDirWatcher::_onChangeNotification(DWORD errorCode, DWORD bytesTransferred) {
	if (_hDir == INVALID_HANDLE_VALUE) return;

//	AX_LOG("bytesTransferred = {}", bytesTransferred);
	if (bytesTransferred > 0) {
		_result.reset();
		_result.path = _path;
		auto span = _buffer.span();

		for (;;) {
			if (bytesTransferred < sizeof(FILE_NOTIFY_INFORMATION)) {
				throw Error_Undefined();
			}
			auto* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(span.data());

			auto& entry = _result.list.emplaceBack();

			 // FileNameLength in byte size
			auto filename = TempString::s_utf(StrViewW(info->FileName, info->FileNameLength / sizeof(WCHAR)));
			FilePath::getUnixPath(entry.filename, filename);

			switch(info->Action) {
				case FILE_ACTION_ADDED:				entry.action = Action::Added;			break;
				case FILE_ACTION_RENAMED_NEW_NAME:	entry.action = Action::RenamedNewName;	break;
				case FILE_ACTION_REMOVED:			entry.action = Action::Removed;			break;
				case FILE_ACTION_RENAMED_OLD_NAME:	entry.action = Action::RemovedOldName;	break;
				case FILE_ACTION_MODIFIED:			entry.action = Action::Modified;		break;
				default:							entry.action = Action::None;			break;
			}

			auto nextOffset = info->NextEntryOffset;;
			if (nextOffset == 0)
				break;

			span = span.sliceFrom(nextOffset);
			bytesTransferred -= nextOffset;
		}
	}

	if (_result.list.size() > 0 && dgResults) {
		dgResults.invoke(_result);
	}

// Start up another ReadDirectoryChangesW

	_buffer.resize(kMaxBufferSize);
	DWORD bytesReturned = 0;

	_overlapped = {};
	_overlapped.hEvent = this; // not used, if CompletionRoutine used, so it's free to use it as user pointer

	BOOL success = ::ReadDirectoryChangesW(
							_hDir, 
							_buffer.data(),
							ax_safe_cast_from(_buffer.size()),
							_watchSubDir, 
							FILE_NOTIFY_CHANGE_CREATION |
							FILE_NOTIFY_CHANGE_FILE_NAME | 
							FILE_NOTIFY_CHANGE_DIR_NAME |
							FILE_NOTIFY_CHANGE_ATTRIBUTES |
							FILE_NOTIFY_CHANGE_SIZE |
							FILE_NOTIFY_CHANGE_LAST_WRITE,
							&bytesReturned,
							&_overlapped,
							&_s_onChangeNotification);
	if (!success)
		throw Error_FileDirWatcher();

}

void FileDirWatcher::create(StrView path, bool watchSubDir) {
	destroy();
	_watchSubDir = watchSubDir;

	_path = FilePath::absPath(path);
	AX_LOG("FileDirWatcher::create({})", _path);

	auto pathW = TempStringW::s_utf(FilePath::winPath(_path));

	_hDir = ::CreateFile(pathW.c_str(),										// Directory to watch
						FILE_LIST_DIRECTORY,									// Access type
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Share mode
						nullptr,												// Security attributes
						OPEN_EXISTING,											// How to create
						FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,		// File attributes
						nullptr);												// Template file

	if (_hDir == INVALID_HANDLE_VALUE) {
		throw Error_FileDirWatcher();
	}

	_onChangeNotification(0, 0);

	::SetTimer(
		nullptr, 
		reinterpret_cast<UINT_PTR>(this),
		200,
		[]( HWND hwnd, UINT msg, UINT_PTR timer, DWORD currentSystemTime) noexcept {
			// Enter an Alertable Wait State: 
			// The completion routine will only be called 
			// when the thread is in an alertable wait state
			::SleepEx(0, true);
		});
}

void FileDirWatcher::destroy() {
	if (_hDir != INVALID_HANDLE_VALUE) {
		::CloseHandle(_hDir);
		_hDir = INVALID_HANDLE_VALUE;
	}

	::KillTimer(nullptr, reinterpret_cast<UINT_PTR>(this));
}

#endif // AX_OS_WINDOWS

} // namespace ax