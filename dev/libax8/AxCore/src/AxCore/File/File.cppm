module;
#include "AxCore-pch.h"

export module AxCore.File;
export import AxCore.Formatter;
export import AxCore.Delegate;
export import AxCore.FilePath;

export namespace ax {

struct FileAttributes {
	bool dir		: 1 = false;
	bool hidden		: 1 = false;
	bool system     : 1 = false;
	bool readonly	: 1 = false;
};

struct FileEntry {
	TempString filename;
	TempString fullpath;
	FileAttributes attr;
};

using FileEntryDelegate = Delegate_<void (FileEntry& entry)>;

struct File {
	File() = delete;

	static Opt<FileAttributes>	getAttrs(StrView filename);
	
	static bool 	exists 					(StrView filename);

	static void		rename					(StrView src_name, StrView dst_name);

	static void		remove					(StrView filename);
	static void		moveToRecycleBin		(StrView filename);

	static void		readBytes				(StrView filename, IByteArray& buf);
	static void		readText				(StrView filename, IString&    buf);
	static void		readUtf8				(StrView filename, IStringA&   buf);

	static void		appendReadBytes			(StrView filename, IByteArray& buf);
	static void		appendReadText			(StrView filename, IString&    buf);
	static void		appendReadUtf8			(StrView filename, IStringA&   buf);

	static void		writeBytes				(StrView filename, ByteSpan buf);
	static void		writeText				(StrView filename, StrView  text);
	static void		writeUtf8				(StrView filename, StrViewA text);

	static void		copy					(StrView dstFilename, StrView srcFilename);

	static void		RevealInExplorer		(StrView path,  bool insideFolder = false);

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

	static void	touch(StrView filename);

	static void glob(StrView searchPath, const FileEntryDelegate& resultDelegate);
};

struct FileDir {
	FileDir() = delete;

	using ListEntryDelegate = Delegate_<void (FileEntry & entry)>;
	static	void	listEntries		(StrView inPath, bool subDir, const ListEntryDelegate& dg);

	static	bool	exists			(StrView path);
	static	void	create			(StrView path, bool subDir = true);
	static	void	remove			(StrView path, bool subDir);
	static	void	removeIfExists	(StrView path, bool subDir);
};

} // namespace 