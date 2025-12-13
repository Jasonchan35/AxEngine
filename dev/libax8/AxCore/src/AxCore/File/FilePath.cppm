module;


export module AxCore.FilePath;
export import AxCore.Formatter;
export import AxCore.Delegate;

export namespace ax {

using FileSize = i64;

enum class FileMode {
	CreateNew,
	OpenExists,
	OpenOrCreate,
};

enum class FileAccess {
	Read,
	ReadWrite,
	WriteOnly,
};

enum class FileShareMode {
	None,
	Read,
	Write,
	ReadWrite,
};

constexpr Int FilePath_kMaxLength = 512;
using FilePathString = String_N<FilePath_kMaxLength>;

struct FilePath {
	FilePath() = delete;

	static constexpr Char kUnixPathSeparator	= '/';
	static constexpr Char kWindowsPathSeparator	= '\\';

#if AX_OS_WINDOWS
	static constexpr Char kNativePathSeparator = kWindowsPathSeparator;
#else
	static constexpr Char kNativePathSeparator = kUnixPathSeparator;
#endif
	
	static constexpr bool isPathSeparator(Char c) { return c == kUnixPathSeparator || c == kNativePathSeparator; }

	struct SplitResult {
		StrView driver;
		StrView dir;
		StrView name;
		StrView ext;
	};

	AX_NODISCARD	static	constexpr SplitResult	split			(StrView path);

	// e.g. changeExtension( "a.txt", ".doc" );
	AX_NODISCARD	static	constexpr FilePathString	changeExtension	(StrView path, StrView ext);

	AX_NODISCARD	static	constexpr 		StrView	dirname_sv	(No_rvalue_<StrView> path);
	AX_NODISCARD	static	constexpr 		StrView	basename_sv	(No_rvalue_<StrView> path, bool withExtension);
	AX_NODISCARD	static	constexpr 		StrView	extension_sv(No_rvalue_<StrView> path);
	
	AX_NODISCARD	static	constexpr FilePathString	dirname		(StrView path)						{ return dirname_sv(path); }
	AX_NODISCARD	static	constexpr FilePathString	basename	(StrView path, bool withExtension)	{ return basename_sv(path, withExtension); }
	AX_NODISCARD	static	constexpr FilePathString	extension	(StrView path)						{ return extension_sv(path); }
	
	static constexpr void getDirname	(IString& outStr, StrView path)						{ outStr = dirname(path); };
	static constexpr void getBasename	(IString& outStr, StrView path, bool withExtension) { outStr = basename(path, withExtension); };
	static constexpr void getExtension	(IString& outStr, StrView path)						{ outStr = extension(path); };
	
	static constexpr void getUnixPath	(IString& outPath, StrView inPath);
	static constexpr void getWinPath	(IString& outPath, StrView inPath);
	static constexpr void getNativePath	(IString& outPath, StrView inPath) {
	#if AX_OS_WINDOWS
		return getWinPath(outPath, inPath);
	#else
		return getUnixPath(outPath, inPath);
	#endif
	}

	AX_NODISCARD static constexpr FilePathString unixPath	(StrView inPath) { FilePathString tmp; getUnixPath(tmp, inPath); return tmp; }
	AX_NODISCARD static constexpr FilePathString winPath	(StrView inPath) { FilePathString tmp; getWinPath( tmp, inPath); return tmp; }
	AX_NODISCARD static constexpr FilePathString nativePath	(StrView inPath) { FilePathString tmp; getNativePath( tmp, inPath); return tmp; }

	AX_NODISCARD static constexpr bool	isAbsPath		(const StrView& path);

	static constexpr void getFullPath	(IString& out_str, StrView dir, StrView path);
	static constexpr void getAbsPath	(IString& out_str, StrView path);
	static constexpr void getRelPath	(IString& out_str, StrView path, StrView relativeTo);

	AX_NODISCARD	static constexpr FilePathString	fullPath	(StrView dir, StrView path)			{ FilePathString tmp; getFullPath(tmp, dir, path);			return tmp; }
	AX_NODISCARD	static constexpr FilePathString	absPath		(StrView path)						{ FilePathString tmp; getAbsPath(tmp, path);				return tmp; }
	AX_NODISCARD	static constexpr FilePathString	relPath		(StrView path, StrView relativeTo)	{ FilePathString tmp; getRelPath(tmp, path, relativeTo);	return tmp; }

	AX_NODISCARD 	static constexpr bool hasPrefix		(StrView path, StrView prefix, StrCase sc = StrCase::Sensitive);

	template<class... ARGS>
	static constexpr void appendTo(IString& outStr, ARGS&&... args) { (_appendTo(outStr, StrView(args)),...); }

	template<class... ARGS>
	static constexpr void set(IString& outStr, ARGS&&... args) { ( outStr.clear(), appendTo(outStr, AX_FORWARD(args)...)); }
	
	static bool setCurrentDir	(StrView path);
	static void getCurrentDir	(IString & path);
	static FilePathString	currentDir		()		{ FilePathString tmp; getCurrentDir(tmp); return tmp; }

	AX_NODISCARD static StrView userAppDataDir		();
	AX_NODISCARD static StrView userLocalAppDataDir	();
	AX_NODISCARD static StrView userDocumentDir		();
	AX_NODISCARD static StrView userHomeDir			();
	AX_NODISCARD static StrView userDesktopDir		();
	AX_NODISCARD static StrView appResourcesDir		();
	AX_NODISCARD static StrView currentProcessDir	();
	AX_NODISCARD static StrView currentProcessFile	();
	AX_NODISCARD static StrView tempDir				();

private:
	static constexpr void _appendTo		(IString& folder, StrView file);
}; // FilePath


constexpr FilePath::SplitResult FilePath::split(StrView path) {
	SplitResult o;

	auto p = path;
	auto s = p.splitByChar(':');
	if (s.second) {
		o.driver = s.first;
		p = s.second;
	}

	s = p.splitByChar_(isPathSeparator);
	if (s.second) {
		o.dir = s.first;
		p = s.second;
	}

	s = p.splitByCharBack('.');
	o.name = s.first;
	o.ext = s.second;

	return o;
}

constexpr StrView FilePath::dirname_sv(No_rvalue_<StrView> path) {
	auto s = path->splitByCharBack_(isPathSeparator);
	return s.second ? s.first : StrView();
}

constexpr StrView FilePath::basename_sv(No_rvalue_<StrView> path, bool withExtension) {
	auto s = path->splitByCharBack_(isPathSeparator);
	auto f = s.second ? s.second : s.first;
	if (withExtension) {
		return f;
	} else {
		s = f.splitByCharBack('.');
		return s.first;
	}
}

constexpr StrView FilePath::extension_sv(No_rvalue_<StrView> path) {
	//	remove dir first to avoid corner case like: "/aaa/bbb/ccc.here/eee"
	//	while should return "" instead or "here/eee"
	auto s = path->splitByCharBack_(isPathSeparator);
	auto f = s.second ? s.second : s.first;

	s = f.splitByCharBack('.');
	return s.second;
}

constexpr void FilePath::getUnixPath(IString& outPath, StrView inPath) {
	outPath = inPath;
	outPath.replaceChars(kWindowsPathSeparator, kUnixPathSeparator);
}

constexpr void FilePath::getWinPath(IString& outPath, StrView inPath) {
	outPath = inPath;
	outPath.replaceChars(kUnixPathSeparator, kWindowsPathSeparator);
}

constexpr FilePathString FilePath::changeExtension(StrView path, StrView new_extension ) {
	FilePathString tmp = dirname(path);

	if (tmp) {
		tmp << "/";
	}
	tmp << basename(path, false);
	if (new_extension) {
		tmp << '.' << new_extension;
	}
	return tmp;
}

constexpr bool FilePath::isAbsPath(const StrView& path) {
	if (path.size() < 1) return false;
	if (path[0] == '/') return true;

	if (path.size() < 2) return false;
	if (CharUtil::isAlpha(path[0]) && path[1] == ':') return true;

	return false;
}

constexpr void FilePath::getFullPath(IString& out_str, StrView dir, StrView path) {
	if (isAbsPath(path)) {
		getAbsPath(out_str, path); // normalize '.' or '..'
	}else{
		FilePathString tmp;
		tmp << dir << '/' << path;
		getAbsPath(out_str, tmp);
	}
}

constexpr void FilePath::getAbsPath(IString& out_str, StrView path) {
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
		auto s = p.splitByChar_(isPathSeparator);
		if (s.first == ".") {
			//skip '.'
		}else if (!s.first) {
			//skip '/'
		}else if (s.first == "..") {
			auto idx = out_str.view().findCharBack_(isPathSeparator);
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

constexpr void FilePath::getRelPath(IString& out_str, StrView path, StrView relativeTo) {
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

constexpr bool FilePath::hasPrefix(StrView path, StrView prefix, StrCase sc) {
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

constexpr void FilePath::_appendTo(IString& folder, StrView file) {
	if (folder.size() != 0) { // avoid "/file"
		auto c = folder.back();
		if (c != '/' && c != '\\') // avoid double '/'
			folder.append('/');
	}
	folder.append(file);
}

} // namespace