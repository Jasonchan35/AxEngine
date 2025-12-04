module AxCore.FilePath;
import AxCore.File;

namespace ax {

FilePath::SplitResult FilePath::split(StrView path) {
	SplitResult o;

	auto p = path;
	auto s = p.splitByChar(':');
	if (s.second) {
		o.driver = s.first;
		p = s.second;
	}

	s = p.splitByAnyCharBack("\\/");
	if (s.second) {
		o.dir = s.first;
		p = s.second;
	}

	s = p.splitByCharBack('.');
	o.name = s.first;
	o.ext = s.second;

	return o;
}

StrView FilePath::dirname ( StrView path ) {
	auto s = path.splitByAnyCharBack("\\/");
	return s.second ? s.first : StrView();
}

StrView FilePath::basename ( StrView path, bool withExtension ) {
	auto s = path.splitByAnyCharBack("\\/");
	auto f = s.second ? s.second : s.first;
	if (withExtension) return f;
	s = f.splitByCharBack('.');
	return s.first;
}

StrView	FilePath::extension ( StrView path ) {
	//	remove dir first to avoid corner case like: "/aaa/bbb/ccc.here/eee"
	//	while should return "" instead or "here/eee"
	auto s = path.splitByAnyCharBack("\\/");
	auto f = s.second ? s.second : s.first;

	s = f.splitByCharBack('.');
	return s.second;
}

void FilePath::getUnixPath(IString& outPath, StrView inPath) {
	outPath = inPath;
	outPath.replaceChars('\\', '/');
}

void FilePath::getWinPath(IString& outPath, StrView inPath) {
	outPath = inPath;
	outPath.replaceChars('/', '\\');
}

TempString FilePath::changeExtension(StrView path, StrView new_extension ) {
	TempString tmp;

	tmp = dirname(path);

	if (tmp) {
		tmp << "/";
	}
	tmp << basename(path, false);
	if (new_extension) {
		tmp << '.' << new_extension;
	}
	return tmp;
}

bool FilePath::isAbsPath(const StrView& path) {
	if (path.size() < 1) return false;
	if (path[0] == '/') return true;

	if (path.size() < 2) return false;
	if (std::isalpha(path[0]) && path[1] == ':') return true;

	return false;
}

void FilePath::getFullPath(IString& out_str, StrView dir, StrView path) {
	if (isAbsPath(path)) {
		getAbsPath(out_str, path); // normalize '.' or '..'
	}else{
		TempString tmp;
		tmp << dir << '/' << path;
		getAbsPath(out_str, tmp);
	}
}

void FilePath::getAbsPath(IString& out_str, StrView path) {
	out_str.clear();

	if (!path) return;
	bool needSlash = false;
	if (isAbsPath(path)) {
		needSlash = (path[0] == '/'); //unix path need '/' at beginning

	}else{
		getCurrentDir(out_str);
		needSlash = true;
	}

	StrView p = path;
	while (p) {
		auto s = p.splitByAnyChar("\\/");
		if (s.first == ".") {
			//skip '.'
		}else if (!s.first) {
			//skip '/'
		}else if (s.first == "..") {
			auto idx = out_str.view().findAnyCharBack("\\/");
			if (!idx) {
				out_str.clear(); //no more parent folder
				return;
			}

			out_str.resize(idx.value());
		}else{
			if (needSlash) {
				out_str << '/';
			}
			out_str << s.first;
			needSlash = true;
		}
		p = s.second;
	}
}

void FilePath::getRelPath(IString& out_str, StrView path, StrView relativeTo) {
	out_str.clear();

	String src = absPath(path);

	String to = absPath(relativeTo);

	auto sv = src.view();
	auto tv = to.view();
	
	// Unix path starts with '/'
	if (sv && sv[0] == '/') sv = sv.sliceFrom(1);
	if (tv && tv[0] == '/') tv = tv.sliceFrom(1);
	
	for(;;) {
		auto sp = sv.splitByChar('/');
		auto tp = tv.splitByChar('/');
		if(!sp.first || !tp.first) break;
		if (!sp.first.equals(tp.first, StrCase::Sensitive)) break;
		
		sv = sp.second;
		tv = tp.second;
	}

	for(;;) {
		auto tp = tv.splitByChar('/');
		if (!tp.first) break;
		out_str.append("../");
		tv = tp.second;
	}

	out_str.append(sv);
}

bool FilePath::hasPrefix(StrView path, StrView prefix, StrCase sc) {
	if (path.startsWith(prefix, sc))
	{
		if (path.size() == prefix.size())
			return true; // exact same path

		auto ch = path[prefix.size()];
		if (ch == '/' || ch == '\\')
			return true; // match folder
	}
	return false;
}

class FilePath_GlobHelper : public NonCopyable {
	using This = FilePath_GlobHelper;
public:
	void search(IArray<String>& outPaths, StrView searchPath, bool needDir, bool needFile, bool needHidden) {
		_needDir    = needDir;
		_needFile   = needFile;
		_needHidden = needHidden;

		_outPaths = &outPaths;
		auto absPath = FilePath::absPath(searchPath);

		auto p = absPath.view();
		auto start = p.splitByAnyChar("*?").first
					  .splitByCharBack('/').first;

		_curPath = start;
		auto remain = p.sliceFrom(start.size() + 1);
		_step(remain);
	}

private:
	void _step(StrView remain) {
		if (!remain) {
			if ((_needFile && File::exists(_curPath))
				||(_needDir && FileDir::exists(_curPath)))
			{
				_outPaths->append(_curPath);
			}
			return;
		}

		auto s = remain.splitByChar('/');
		_step2(s.first, s.second);
	}

	void _step2(StrView name, StrView remain) {
		if (!FileDir::exists(_curPath))
			return;

		if (name == "**") {
			_step(remain);
		}

		class Callback : public FileDir::OnEntryCallback {
			This* _owner = nullptr;
			StrView _name;
			StrView _remain;
		public:
			Callback(This* owner, StrView name, StrView remain) : _owner(owner), _name(name), _remain(remain) {}
			virtual void onEntry(FileDirEntry& entry) override {
				_owner->_onFileEntry(entry, _name, _remain);
			}
		};

		Callback callback(this, name, remain);
		FileDir::listEntries(_curPath, false, callback);
	};
	
	void _onFileEntry(FileDirEntry& entry, StrView name, StrView remain) {
		if (!_needHidden && entry.name.startsWith("."))
			return;

		if (entry.isDir && name == "**") {
			auto oldSize = _curPath.size();
			_curPath << '/' << entry.name;

			_step2(name, remain);
			// back to last level
			_curPath.resize(oldSize);

		} else if (entry.name.matchWildcard(name, StrCase::Sensitive)) {
			auto oldSize = _curPath.size();
			_curPath << '/' << entry.name;

			_step(remain);

			// back to last level
			_curPath.resize(oldSize);
		}
	}	

	IArray<String>* _outPaths = nullptr;
	String _curPath;
	bool _needDir    = true;
	bool _needFile   = true;
	bool _needHidden = false;
};

void FilePath::glob(IArray<String>& outPaths, StrView searchPath, bool needDir, bool needFiles, bool needHidden) {
	outPaths.clear();
	appendGlob(outPaths, searchPath, needDir, needFiles, needHidden);
}

void FilePath::appendGlob(IArray<String>& outPaths, StrView searchPath, bool needDir, bool needFiles, bool needHidden) {
	auto f = searchPath.findAnyChar("*?");
	if (!f) {
		outPaths.emplaceBack(searchPath);
		return;
	}

	FilePath_GlobHelper helper;
	helper.search(outPaths, searchPath, needDir, needFiles, needHidden);
}

void FilePath::append(IString& folder, StrView file) {
	if (folder.size() != 0) { // avoid "/file"
		auto c = folder.back();
		if (c != '/' && c != '\\') // avoid double '/'
			folder.append('/');
	}
	folder.append(file);
}

StrView FilePath::appResourcesDir() {
	struct Obj {
		Obj() {
			TempString tmp(currentProcessDir());
		#if AX_OS_MACOSX
			tmp.append("/../Resources");
		#endif
			FilePath::getAbsPath(path, tmp);
		}
		TempString path;
	};

	static Obj obj;
	return obj.path;
}

StrView FilePath::currentProcessDir() {
	return dirname(currentProcessFile());
}

#if 0
#pragma mark ================= AX_OS_WINDOWS ====================
#endif
#if AX_OS_WINDOWS

#include <ShlObj_core.h>

void FilePath::getCurrentDir(IString & path) {
	StringW_N<FilePath::kMaxChar> w;
	w.resizeToLocalBufSize();
	auto n = GetCurrentDirectory(static_cast<DWORD>(w.size()), w.data());
	w.resize(n);
	path.setUtf(w);
	path.replaceChars('\\', '/');
}

bool FilePath::setCurrentDir(StrView path){
	TempStringW w;
	w.setUtf(path);
	return SetCurrentDirectory(w.c_str());
}

namespace {
struct FilePath_SHGetFolderPath {
	FilePath_SHGetFolderPath(int CSIDL) {
		wchar_t	p[MAX_PATH + 1];
		HRESULT ret = SHGetFolderPath(nullptr, CSIDL | CSIDL_FLAG_DONT_VERIFY, nullptr, SHGFP_TYPE_CURRENT, p);
		if( ret != S_OK ) throw Error_Undefined(AX_SRC_LOC);	
		path = FilePath::absPath(TempString::s_utf(StrView_c_str(p)));
	}
	TempString	path;
};
} // namespace

StrView FilePath::userAppDataDir		() { static FilePath_SHGetFolderPath s(CSIDL_APPDATA);			return s.path; }
StrView FilePath::userLocalAppDataDir	() { static FilePath_SHGetFolderPath s(CSIDL_LOCAL_APPDATA);	return s.path; }
StrView FilePath::userDocumentDir		() { static FilePath_SHGetFolderPath s(CSIDL_MYDOCUMENTS);		return s.path; }
StrView FilePath::userHomeDir			() { static FilePath_SHGetFolderPath s(CSIDL_PROFILE);			return s.path; }
StrView FilePath::userDesktopDir		() { static FilePath_SHGetFolderPath s(CSIDL_DESKTOP);			return s.path; }

StrView FilePath::currentProcessFile() {
	struct Obj {
		Obj() {
			FixedArray<wchar_t, MAX_PATH + 1> tmp;
			DWORD n = ::GetModuleFileName(nullptr, tmp.data(), MAX_PATH);
			if (n == 0) throw Error_Undefined(AX_SRC_LOC);
			tmp[n] = 0;
			path.setUtf(StrView_ref(tmp.slice(0, n)));
		}
		TempString path;
	};
	static Obj obj;
	return obj.path;
}

StrView FilePath::tempDir() {
	struct Obj {
		Obj() {
			FixedArray<wchar_t, MAX_PATH + 1> tmp;
			tmp[MAX_PATH] = 0;
			DWORD n = ::GetTempPath(MAX_PATH, tmp.data());
			if (n == 0) throw Error_Undefined(AX_SRC_LOC);
			tmp[n] = 0;
			path.setUtf(StrView_ref(tmp.slice(0, n)));
		}
		TempString path;
	};
	static Obj obj;
	return obj.path;
}

#endif // AX_OS_WINDOWS


#if 0
#pragma mark ================= iOS / Mac ====================
#endif

#if AX_OS_IOS || AX_OS_MACOSX

class axPath_NSSearchPath {
public:
	axPath_NSSearchPath(NSSearchPathDirectory d) {
		NSArray *tmp = NSSearchPathForDirectoriesInDomains(d, NSUserDomainMask, YES);
		if (tmp == nil) throw Error_Undefined(AX_SRC_LOC);
		if ([tmp count] <= 0) throw Error_Undefined(AX_SRC_LOC);
		NSString *docu = [tmp objectAtIndex:0];
		if (docu == nil) throw Error_Undefined(AX_SRC_LOC);

		path.set(StrView_ref(docu));
	}

	TempString path;
};

StrView FilePath::userAppDataDir	() { static axPath_NSSearchPath s(NSApplicationSupportDirectory);	return s.path; }
StrView FilePath::userDocumentDir	() { static axPath_NSSearchPath s(NSDocumentDirectory);				return s.path; }
StrView FilePath::userDesktopDir	() { static axPath_NSSearchPath s(NSDesktopDirectory);				return s.path; }

StrView FilePath::userHomeDir		() {
	struct Obj {
		Obj() {
			path.set(StrView_make(NSHomeDirectory()));
		}
		TempString path;
	};
	static Obj obj;
	return obj.path;
}

StrView FilePath::currentProcessFile() {
	struct Obj {
		Obj() {
			NSProcessInfo* info = [NSProcessInfo processInfo];
			NSArray* args = [info arguments];
			if ([args count ] == 0) throw Error_Undefined(AX_SRC_LOC);

			auto s = (NSString*)[args objectAtIndex:0];
			path.set(StrView_make(s));
		}
		TempString path;
	};
	static Obj obj;
	return obj.path;
}

#endif // iOS / Mac


#if 0
#pragma mark ================= Linux ====================
#endif

#if AX_OS_LINUX

#include <linux/limits.h>

StrView FilePath::userAppDataDir() {
	struct Obj {
		Obj() {
			FmtTo(path, "{}/{}", userHomeDir(), basename(currentProcessFile(), false));
		}
		TempString path;
	};
	static const Obj obj;
	return obj.path;
}

StrView FilePath::userDocumentDir() {
	struct Obj {
		Obj() {
			path = userHomeDir();
			path.append("/Documents");
		}
		TempString path;
	};
	static Obj obj;
	return obj.path;
}

StrView FilePath::userDesktopDir() {
	struct Obj {
		Obj() {
			path = userHomeDir();
			path.append("/Desktop");
		}
		TempString path;
	};
	static Obj obj;
	return obj.path;
}

StrView FilePath::userHomeDir() {
	struct Obj {
		Obj() {
			struct passwd	*p = ::getpwuid( ::getuid() );
			if (!p) throw Error_Undefined(AX_SRC_LOC);
			path.setUtf(StrView_c_str(p->pw_dir));
		}
		TempString path;
	};
	static Obj obj;
	return obj.path;
}

StrView FilePath::currentProcessFile() {
	struct Obj {
		Obj() {
			char buf[PATH_MAX + 1];
			ssize_t ret = readlink("/proc/self/exe", buf, PATH_MAX);
			if (ret == -1) throw Error_Undefined(AX_SRC_LOC);

			// if the /proc/self/exe symlink has been dropped by the kernel for
			// some reason, then readlink() could also return success but
			// "(deleted)" as link destination...
			if (ZStrUtil::equals(buf, "(deleted)")) {
				 throw Error_Undefined(AX_SRC_LOC);
			}

			buf[ret] = 0;
			path.setUtf(StrView_c_str(buf));
		}
		TempString path;
	};
	static Obj obj;
	return obj.path;
}

#endif //axOS_Linux

#if 0
#pragma mark ================= Unix ====================
#endif
#if AX_OS_UNIX || AX_OS_CYGWIN || AX_OS_MINGW

void FilePath::getCurrentDir(IString& path) {
	path.clear();
	char  tmp[ FilePath::kMaxChar + 1 ];
	if( ! ::getcwd( tmp, FilePath::kMaxChar ) ) {
		throw Error_Undefined(AX_SRC_LOC);
	}
	tmp[ FilePath::kMaxChar ] = 0;
	path.setUtf(StrView_c_str(tmp));
}

bool FilePath::setCurrentDir(StrView path) {
	TempStringA pathA;
	pathA.setUtf(path);
	return 0 == ::chdir(pathA.c_str());
}

StrView FilePath::tempDir() {
	return "/tmp";
}


#endif // Unix


} // namespace 