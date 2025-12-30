module;

module AxCore.FilePath;
import AxCore.File;
import AxCore.Array;

namespace ax {

StrView FilePath::currentProcessDir() {
	return dirname(currentProcessFile());
}

StrView FilePath::appResourcesDir() {
	struct Obj {
		Obj() {
			FilePathString tmp(currentProcessDir());
#if AX_OS_MACOSX
			tmp.append("/../Resources");
#endif
			FilePath::getAbsPath(path, tmp);
		}
		FilePathString path;
	};

	static Obj obj;
	return obj.path;
}

#if 0
#pragma mark ================= AX_OS_WINDOWS ====================
#endif
#if AX_OS_WINDOWS

void FilePath::getCurrentDir(IString & path) {
	StringW_N<FilePath_kMaxLength> w;
	w.resizeToCapacity();
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
		if( ret != S_OK ) throw Error_Undefined();
		path = FilePath::absPath(FilePathString::s_utf(StrView_c_str(p)));
	}
	FilePathString	path;
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
			if (n == 0) throw Error_Undefined();
			tmp[n] = 0;
			path.setUtf(StrView_ref(tmp.slice(0, n)));
		}
		FilePathString path;
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
			if (n == 0) throw Error_Undefined();
			tmp[n] = 0;
			path.setUtf(StrView_ref(tmp.slice(0, n)));
		}
		FilePathString path;
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
		if (tmp == nil) throw Error_Undefined();
		if ([tmp count] <= 0) throw Error_Undefined();
		NSString *docu = [tmp objectAtIndex:0];
		if (docu == nil) throw Error_Undefined();

		path.set(StrView_ref(docu));
	}

	FilePathString path;
};

StrView FilePath::userAppDataDir	() { static axPath_NSSearchPath s(NSApplicationSupportDirectory);	return s.path; }
StrView FilePath::userDocumentDir	() { static axPath_NSSearchPath s(NSDocumentDirectory);				return s.path; }
StrView FilePath::userDesktopDir	() { static axPath_NSSearchPath s(NSDesktopDirectory);				return s.path; }

StrView FilePath::userHomeDir		() {
	struct Obj {
		Obj() {
			path.set(StrView_make(NSHomeDirectory()));
		}
		FilePathString path;
	};
	static Obj obj;
	return obj.path;
}

StrView FilePath::currentProcessFile() {
	struct Obj {
		Obj() {
			NSProcessInfo* info = [NSProcessInfo processInfo];
			NSArray* args = [info arguments];
			if ([args count ] == 0) throw Error_Undefined();

			auto s = (NSString*)[args objectAtIndex:0];
			path.set(StrView_make(s));
		}
		FilePathString path;
	};
	static Obj obj;
	return obj.path;
}

#endif // iOS / Mac


#if 0
#pragma mark ================= Linux ====================
#endif

#if AX_OS_LINUX

StrView FilePath::userAppDataDir() {
	struct Obj {
		Obj() {
			FmtTo(path, "{}/{}", userHomeDir(), basename(currentProcessFile(), false));
		}
		FilePathString path;
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
		FilePathString path;
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
		FilePathString path;
	};
	static Obj obj;
	return obj.path;
}

StrView FilePath::userHomeDir() {
	struct Obj {
		Obj() {
			struct passwd	*p = ::getpwuid( ::getuid() );
			if (!p) throw Error_Undefined();
			path.setUtf(StrView_c_str(p->pw_dir));
		}
		FilePathString path;
	};
	static Obj obj;
	return obj.path;
}

StrView FilePath::currentProcessFile() {
	struct Obj {
		Obj() {
			char buf[PATH_MAX + 1];
			ssize_t ret = readlink("/proc/self/exe", buf, PATH_MAX);
			if (ret == -1) throw Error_Undefined();

			// if the /proc/self/exe symlink has been dropped by the kernel for
			// some reason, then readlink() could also return success but
			// "(deleted)" as link destination...
			if (ZStrUtil::equals(buf, "(deleted)")) {
				 throw Error_Undefined();
			}

			buf[ret] = 0;
			path.setUtf(StrView_c_str(buf));
		}
		FilePathString path;
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
	char  tmp[ FilePath_kMaxLangth + 1 ];
	if( ! ::getcwd( tmp, FilePath_kMaxLangth ) ) {
		throw Error_Undefined();
	}
	tmp[ FilePath_kMaxLangth ] = 0;
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