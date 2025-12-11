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

constexpr Int FilePath_kMaxLangth = 512;
using FilePathString = String_N<FilePath_kMaxLangth>;

struct FilePath {
	FilePath() = delete;

	static constexpr Char kUnixPathSeperator	= '/';
	static constexpr Char kWindowsPathSeperator	= '\\';

#if AX_OS_WINDOWS
	static constexpr Char kNativePathSeperator = kWindowsPathSeperator;
#else
	static constexpr Char kNativePathSeperator = kUnixPathSeperator;
#endif
	
	static constexpr bool isPathSeperator(Char c) { return c == kUnixPathSeperator || c == kNativePathSeperator; }

	struct SplitResult {
		StrView driver;
		StrView dir;
		StrView name;
		StrView ext;
	};

	AX_NODISCARD	static	SplitResult	split			(StrView path);

	// e.g. changeExtension( "a.txt", ".doc" );
	AX_NODISCARD	static	FilePathString	changeExtension	(StrView path, StrView ext);

	AX_NODISCARD	static			StrView	dirname_sv	(No_rvalue_<StrView> path);
	AX_NODISCARD	static			StrView	basename_sv	(No_rvalue_<StrView> path, bool withExtension);
	AX_NODISCARD	static			StrView	extension_sv(No_rvalue_<StrView> path);
	
	AX_NODISCARD	static	FilePathString	dirname		(StrView path)						{ return dirname_sv(path); }
	AX_NODISCARD	static	FilePathString	basename	(StrView path, bool withExtension)	{ return basename_sv(path, withExtension); }
	AX_NODISCARD	static	FilePathString	extension	(StrView path)						{ return extension_sv(path); }
	
	static	void	getDirname		(IString& outStr, StrView path)						{ outStr = dirname(path); };
	static	void	getBasename		(IString& outStr, StrView path, bool withExtension) { outStr = basename(path, withExtension); };
	static	void	getExtension	(IString& outStr, StrView path)						{ outStr = extension(path); };
	
	static void getUnixPath(IString& outPath, StrView inPath);
	static void getWinPath(IString& outPath, StrView inPath);
	static void getNativePath(IString& outPath, StrView inPath) {
	#if AX_OS_WINDOWS
		return getWinPath(outPath, inPath);
	#else
		return getUnixPath(outPath, inPath);
	#endif
	}

	AX_NODISCARD	static	FilePathString	unixPath		(StrView inPath) { FilePathString tmp; getUnixPath(tmp, inPath); return tmp; }
	AX_NODISCARD	static	FilePathString	winPath			(StrView inPath) { FilePathString tmp; getWinPath( tmp, inPath); return tmp; }
	AX_NODISCARD	static	FilePathString	nativePath		(StrView inPath) { FilePathString tmp; getNativePath( tmp, inPath); return tmp; }

	AX_NODISCARD	static	bool		isAbsPath		(const StrView& path);

	static	void		getFullPath		(IString& out_str, StrView dir, StrView path);
	static	void		getAbsPath		(IString& out_str, StrView path);
	static	void		getRelPath		(IString& out_str, StrView path, StrView relativeTo);

	AX_NODISCARD	static	FilePathString	fullPath		(StrView dir, StrView path)				{ FilePathString tmp; getFullPath(tmp, dir, path);			return tmp; }
	AX_NODISCARD	static	FilePathString	absPath			(StrView path)							{ FilePathString tmp; getAbsPath(tmp, path);				return tmp; }
	AX_NODISCARD	static	FilePathString	relPath			(StrView path, StrView relativeTo)		{ FilePathString tmp; getRelPath(tmp, path, relativeTo);	return tmp; }

	AX_NODISCARD 	static	bool		hasPrefix		(StrView path, StrView prefix, StrCase sc = StrCase::Sensitive);

	template<class... ARGS>
	static  void		appendTo(IString& outStr, ARGS&&... args) { (_appendTo(outStr, StrView(args)),...); }

	template<class... ARGS>
	static  void		set(IString& outStr, ARGS&&... args) { ( outStr.clear(), appendTo(outStr, AX_FORWARD(args)...)); }
	
	static	bool		setCurrentDir	(StrView path);
	static	void		getCurrentDir	(IString & path);
	static  FilePathString	currentDir		()		{ FilePathString tmp; getCurrentDir(tmp); return tmp; }

	AX_NODISCARD	static	StrView		userAppDataDir		();
	AX_NODISCARD	static	StrView		userLocalAppDataDir	();
	AX_NODISCARD	static	StrView		userDocumentDir		();
	AX_NODISCARD	static	StrView		userHomeDir			();
	AX_NODISCARD	static	StrView		userDesktopDir		();
	AX_NODISCARD	static	StrView		appResourcesDir		();
	AX_NODISCARD	static	StrView		currentProcessDir	();
	AX_NODISCARD	static	StrView		currentProcessFile	();
	AX_NODISCARD	static	StrView		tempDir				();

private:
	static	void		_appendTo		(IString& folder, StrView file);
}; // FilePath
	
} // namespace