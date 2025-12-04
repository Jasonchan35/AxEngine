module;
#include "AxCore-pch.h"

export module AxCore.File;
export import AxCore.Formatter;
export import AxCore.Delegate;

export namespace ax {

struct File {
	File() = delete;

	static bool 	exists 					( StrView filename );

	static void		rename					( StrView src_name, StrView dst_name );

	static void		remove					( StrView filename );
	static void		moveToRecycleBin		( StrView filename );

	static void		readBytes				( StrView filename, IByteArray& buf );
	static void		readText				( StrView filename, IString&    buf );
	static void		readUtf8				( StrView filename, IStringA&   buf );

	static void		appendReadBytes			( StrView filename, IByteArray& buf );
	static void		appendReadText			( StrView filename, IString&    buf );
	static void		appendReadUtf8			( StrView filename, IStringA&   buf );

	static void		writeBytes				( StrView filename, ByteSpan buf );
	static void		writeText				( StrView filename, StrView  text );
	static void		writeUtf8				( StrView filename, StrViewA text );

	static void		copy					(StrView dstFilename, StrView srcFilename);

	static void		RevealInExplorer		( StrView path,  bool insideFolder = false);

	enum WriteFileResult {
		None,
		NoChange,
		NewFile,
		Updated,
	};

	static WriteFileResult	writeFile			( StrView filename, ByteSpan buf,  bool createDir, bool logResult = true);
	static WriteFileResult	writeFile			( StrView filename, StrViewA text, bool createDir, bool logResult = true);

	static WriteFileResult	writeFileIfChanged	( StrView filename, ByteSpan buf,  bool createDir, bool logResult = true, bool logNoChange = false);
	static WriteFileResult	writeFileIfChanged	( StrView filename, StrViewA text, bool createDir, bool logResult = true, bool logNoChange = false);

	static void		touch	(StrView filename);
};

struct FileDirEntry {
	TempString name;
	TempString path;
	bool isDir  = false;
	bool hidden = false;
};

struct FileDir {
	FileDir() = delete;

	class OnEntryCallback {
	public:
		virtual ~OnEntryCallback() = default;
		virtual void onEntry(FileDirEntry & entry) = 0;
	};

	static	void	listEntries		(StrView inPath, bool subDir, OnEntryCallback& callback);

	static	bool	exists			(StrView path);
	static	void	create			(StrView path, bool subDir = true);
	static	void	remove			(StrView path, bool subDir);
	static	void	removeIfExists	(StrView path, bool subDir);
};

} // namespace 