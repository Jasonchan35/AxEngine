module;

#include "AxCore-pch.h"

module AxCore.File;
import AxCore.FileStream;
import AxCore.Logger;
import AxCore.FileMemMap;

namespace ax {

#if 0
#pragma mark --------- AX_OS_UNIX ---------
#endif
#if AX_OS_UNIX

void File::remove	( StrView filename ) {
	auto filenameA = TempStringA::s_utf(filename);
	auto ret = ::remove( filenameA.c_str() );
	if( ret != 0 ) {
		throw Error_Undefined(AX_SRC_LOC);
	}
}

bool File::exists ( StrView filename ) {
	auto filenameA = TempStringA::s_utf(filename);
	struct stat s;
	if( 0 != ::stat( filenameA.c_str(), &s ) ) return false;
	return ( s.st_mode & S_IFDIR ) == 0;
}

void File::rename	( StrView src_name, StrView dst_name ) {
	auto src = TempStringA::s_utf(src_name);
	auto dst = TempStringA::s_utf(dst_name);
	auto ret = ::rename( src.c_str(), dst.c_str() );
	if( ret != 0 ) {
		throw Error_Undefined(AX_SRC_LOC);
	}
}

void File::RevealInExplorer(StrView path, bool insideFolder) {
#if AX_OS_MACOSX
	auto fullpath =	FilePath::fullPath(path);
	auto cmd	  = Fmt("open -R \"{}\"", fullpath);
	std::system(cmd.c_str());
#else
	// #error ""
	AX_ASSERT(false);
#endif
}


bool FileDir::exists( StrView path ) {
	TempStringA pathA;
	pathA.setUtf(path);

	struct stat s;
	if( 0 != ::stat( pathA.c_str(), &s ) ) return false;
	return ( s.st_mode & S_IFDIR ) != 0;
}

void	Dir_os_create	( StrView path ) {
	TempStringA pathA;
	pathA.setUtf(path);

#if AX_OS_MINGW
	auto ret = ::mkdir( pathA.c_str() );
#else
	auto ret = ::mkdir( pathA.c_str(), 0755 );
#endif

	if( ret != 0 ) {
		if( errno == EEXIST ) return; //already exists
		throw Error_File();
	}
}

void Dir_os_remove(StrView path) {
	TempStringA pathA;
	pathA.setUtf(path);

	auto ret = ::rmdir( pathA.c_str() );
	if( ret != 0 ) {
		throw Error_File();
	}
}

void FileDir::listEntries(StrView path, bool subDir, FilterFunc filter) {
	TempStringA pathA;
	pathA.setUtf(path);

	struct dirent*	r = nullptr;

	auto h = ::opendir( pathA.c_str() );
	if( ! h ) {
		throw Error_File();
	}

	try{
		FileDir::Entry entry;

		for(;;) {

#if 1 // AX_OS_MINGW
			r = ::readdir( h );
#else
			struct dirent	entry;
			if( 0 != ::readdir_r( h, &entry, &r ) ) {
				throw Error_Undefined(AX_SRC_LOC);
			}
#endif

			if( ! r ) break;

			entry.hidden = ZStrUtil::startsWith(r->d_name, ".");

			if (ZStrUtil::equals(r->d_name, "." )) continue;
			if (ZStrUtil::equals(r->d_name, "..")) continue;

#if AX_OS_MINGW || AX_OS_CYGWIN
			struct stat statbuf;
			TempString8 tmpFullpath;
			tmpFullpath.set(path);
			tmpFullpath.append_c_str(r->d_name);
			if (stat(r->d_name, &statbuf) == -1) {
				throw Error_Undefined(AX_SRC_LOC);
			}
			entry.isDir = S_ISDIR(statbuf.st_mode);
#else
			entry.isDir = ( r->d_type & DT_DIR );
#endif

			entry.name.setUtf(StrView_c_str(r->d_name));
			entry.path.set(path);
			FilePath::append(entry.path, entry.name);

			filter(entry);

			if(entry.isDir && subDir) {
				listEntries(entry.name, subDir, filter );
			}
		}
	} catch (...) {
		::closedir(h);
		throw;
	}
}

} // namespace


#endif // UNIX


#if 0
#pragma mark --------- AX_OS_WINDOWS ---------
#endif
#if AX_OS_WINDOWS

void File::remove	( StrView filename ) {
	TempStringW filenameW;
	filenameW.setUtf(filename);
	if( ! ::DeleteFile( filenameW.c_str()) ) {
		throw Error_File();
	}
}

void File::moveToRecycleBin	( StrView filename ) {
	TempStringW filenameW;
	filenameW.setUtf(filename);
	filenameW.append(L'\0');	//double 0 end is needed by WinAPI

	SHFILEOPSTRUCT op = {};
	op.hwnd		= nullptr;
	op.wFunc	= FO_DELETE;
	op.pFrom	= filenameW.c_str();
	op.pTo		= nullptr;
	op.fFlags	= FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;

	if( 0 != SHFileOperation( &op ) ) throw Error_File();
}

void File::RevealInExplorer(StrView path, bool insideFolder) {
	if (!File::exists(path) && !FileDir::exists(path)) return;

	auto fullpath = FilePath::absPath(path);

	TempString cmd = "explorer ";
	if (!insideFolder) {
		cmd << "/select,";
	}
	cmd << "\"";
	cmd << FilePath::winPath(fullpath);
	cmd << "\"";

	std::system(cmd.c_str());
}


bool File::exists ( StrView filename ) {
	TempStringW filenameW;
	filenameW.setUtf(filename);
	DWORD dwAttrib = ::GetFileAttributes(filenameW.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void File::rename	( StrView src_name, StrView dst_name ) {
	TempStringW src;
	src.setUtf(src_name);
	TempStringW dst;
	dst.setUtf(dst_name);
	if( 0 != ::_wrename( src.c_str(), dst.c_str() ) ) {
		throw Error_File();
	}
}

static void Dir_os_remove(StrView path) {
	TempStringW pathW;
	pathW.setUtf(path);
	if (0 == ::RemoveDirectory(pathW.c_str())) {
		throw Error_File();
	}
}

static void Dir_os_create(StrView path) {
	TempStringW pathW;
	pathW.setUtf(path);
	if (0 == ::CreateDirectory(pathW.c_str(), nullptr)) {
		throw Error_File();
	}
}

bool FileDir::exists(StrView path) {
	TempStringW pathW;
	pathW.setUtf(path);
	DWORD dwAttrib = ::GetFileAttributes(pathW.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

class Dir_Win32Handle : public NonCopyable {
public:
	Dir_Win32Handle(HANDLE h = INVALID_HANDLE_VALUE) : _h(h) {}
	~Dir_Win32Handle() { unref(); }

	void set(HANDLE h) { unref(); _h = h; }
	void unref() { if(isValid()) {::FindClose(_h); _h = INVALID_HANDLE_VALUE; } }

	HANDLE	get() { return _h; }
	operator HANDLE() { return _h; }

	bool isValid() const { return _h != INVALID_HANDLE_VALUE; }

private:
	HANDLE _h;
};

void FileDir::listEntries(StrView inPath, bool subDir, OnEntryCallback& callback) {
	// because 'filter' callback may change the path string outside
	TempString  path = inPath;
	auto pathW = TempStringW::s_format(L"{}/*", inPath);

	WIN32_FIND_DATA data;
	Dir_Win32Handle h(::FindFirstFile(pathW.c_str(), &data));
	if (!h.isValid()) {
		throw Error_File();
	}

	do{
		auto filename = StrView_c_str(data.cFileName);
		if (filename == L"." || filename == L"..") continue;

		FileDirEntry entry;
		entry.name.setUtf(filename);
		entry.path.set(path);
		FilePath::append(entry.path, entry.name);
		
		entry.isDir  = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
		entry.hidden = (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN   ) ? true : false;

		callback.onEntry(entry);

		if(entry.isDir && subDir) {
			listEntries(entry.name, subDir, callback);
		}
	}while(::FindNextFile(h, &data));

	auto err = ::GetLastError();
	if (err != ERROR_NO_MORE_FILES) {
		throw Error_File();
	}
}

#endif // windows


void File::readText( StrView filename, IString & buf ) {
	buf.clear();
	appendReadText(filename, buf);
}

void File::readUtf8( StrView filename, IStringA & buf ) {
	buf.clear();
	appendReadUtf8(filename, buf);
}

void File::readBytes(StrView filename, IByteArray& buf) {
	buf.clear();
	appendReadBytes(filename, buf);
}

void File::appendReadText ( StrView filename, IString & buf ) {
	FileStream	s;
	s.openRead( filename );
	s.appendReadAllText( buf );
}

void File::appendReadBytes(StrView filename, IByteArray& buf) {
	FileStream	s;
	s.openRead( filename );
	s.appendReadAllBytes( buf );
}

void File::appendReadUtf8 ( StrView filename, IStringA & buf ) {
	FileStream	s;
	s.openRead( filename );
	s.appendReadAllUtf8( buf );
}

void File::writeBytes(StrView filename, ByteSpan buf) {
	FileStream	s;
	s.openWrite( filename, true );
	s.writeBytes( buf );
}

void File::writeText		( StrView filename, StrView text ) {
	FileStream	s;
	s.openWrite( filename, true );
	s.writeText( text );
}

void File::writeUtf8		( StrView filename, StrViewA text ) {
	FileStream	s;
	s.openWrite( filename, true );
	s.writeUtf8( text );
}

File::WriteFileResult File::writeFile(StrView filename, ByteSpan buf, bool createDir, bool logResult) {
	Char op = '+';
	auto res = WriteFileResult::NewFile;

	auto absPath = FilePath::absPath(filename);

	if (File::exists(absPath)) {
		op = 'U';
		res = WriteFileResult::Updated;
	}

	if (logResult) {
		AX_LOG("[{}] {}, size={}", op, absPath, buf.sizeInBytes());
	}

	if (createDir) {
		if (auto dir = FilePath::dirname(absPath)) {
			FileDir::create(dir, true);
		}
	}

	File::writeBytes(absPath, buf);
	return res;
}

File::WriteFileResult File::writeFile(StrView filename, StrViewA text, bool createDir, bool logResult) {
	return writeFile(filename, text.toByteSpan(), createDir, logResult);
}

File::WriteFileResult File::writeFileIfChanged(StrView filename, ByteSpan buf, bool createDir, bool logResult, bool logNoChange) {
	Char op = '+';
	AX_UNUSED(op);

	auto res = WriteFileResult::NewFile;

	auto absPath = FilePath::absPath(filename);

	if (File::exists(absPath)) {
		FileMemMap map(absPath);
		if (map.span() == buf) {
			op = '=';
			res = WriteFileResult::NoChange;
		}else{
			op = 'U';
			res = WriteFileResult::Updated;
		}
	}

	if (logResult) {
		if (res != WriteFileResult::NoChange || logNoChange) {
			AX_LOG("[{}] {}, size={}", op, absPath, buf.sizeInBytes());
		}
	}

	if (res == WriteFileResult::NoChange) return res;
	File::writeFile(absPath, buf, createDir, false);
	return res;
}

File::WriteFileResult File::writeFileIfChanged(StrView filename, StrViewA text, bool createDir, bool logResult, bool logNoChange) {
	return writeFileIfChanged(filename, text.toByteSpan(), createDir, logResult, logNoChange);
}

void File::touch(StrView filename) {
	FileStream f;
	f.openWrite(filename, false);

	UtcTime t;
	t.setToNow();
	f.setLastWriteTime(t);
}

void File::copy(StrView dstFilename, StrView srcFilename) {
	FileMemMap	mm(srcFilename);
	writeFile(dstFilename, mm.span(), true, true);
}


void FileDir::removeIfExists(StrView path, bool subDir) {
	if (FileDir::exists(path)) {
		FileDir::remove(path, subDir);
	}
}

void FileDir::remove(StrView path, bool subDir) {
	if (!subDir) return Dir_os_remove(path);

	class Callback : public FileDir::OnEntryCallback {
		bool _subDir;
	public:
		Callback(bool subdir) : _subDir(subdir) {}
		virtual void onEntry(FileDirEntry& entry) override {
			if (entry.isDir) {
				FileDir::remove(entry.name, _subDir);
			} else {
				File::remove(entry.name);
			}
		}
	};

	Callback callback(subDir);
	listEntries(path, false, callback);

	Dir_os_remove(path);
}

void FileDir::create(StrView path, bool subDir) {
	if (exists(path)) return;

	if (subDir) {
		if (auto parent = FilePath::dirname(path)) {
			create(parent, subDir);
		}
	}

	Dir_os_create(path);
}

} // namespace

