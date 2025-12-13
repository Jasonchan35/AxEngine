module;


export module AxCore.FileStream;
export import AxCore.FilePath;
export import AxCore.TimeStamp;

export namespace ax {


class FileStream : public NonCopyable {
public:
	void		openRead			( StrView filename);
	void		openAppend			( StrView filename);
	void		openWrite			( StrView filename, bool truncate);

	void		open				( StrView filename, FileMode mode, FileAccess access, FileShareMode share );

	UtcTime		lastAccessTime 		();
	UtcTime		lastWriteTime		();
	void		setLastWriteTime	(const UtcTime& t);

	void		close				();
	bool		isOpened			() const;

	void		lock				( bool exclusive );
	bool		tryLock				( bool exclusive );
	void		unlock				();

	void		setPos				( FileSize n );
	void		advPos				( FileSize n ); //!< advance position
	void		setPosEnd			( FileSize n ); //!< set position from end

	FileSize	getPos				();
	FileSize	getFileSize			();
	void		setFileSize			( FileSize n );

//------
	void		readBytes			(MutByteSpan buf);
	void		writeBytes			(   ByteSpan buf);

	void		writeText			( StrView buf );
	void		writeUtf8			( const StrViewA& buf );

	void		readAllBytes		( IByteArray& buf )	{ buf.clear(); appendReadAllBytes(buf); }
	void		appendReadAllBytes	( IByteArray& buf );

	void		readAllText			( IString& buf )	{ buf.clear(); appendReadAllText(buf); }
	void		appendReadAllText	( IString& buf );

	void		readAllUtf8			( IStringA & buf )	{ buf.clear(); appendReadAllUtf8(buf); }
	void		appendReadAllUtf8	( IStringA & buf );

	void		flush			();

	StrView	filename		() const { return _filename; }

	FileStream();
	FileStream(FileStream && r) noexcept { operator=(AX_FORWARD(r)); }

	~FileStream();

	void operator=(FileStream && r) noexcept;

#if AX_OS_WINDOWS
public:
	const HANDLE kInvalidHandle = INVALID_HANDLE_VALUE;
	HANDLE		nativeFileDescriptor()	{ return _fd; }
private:

	bool		_os_lock	( DWORD flags );
	HANDLE		_fd = kInvalidHandle;
#endif

#if AX_OS_UNIX || AX_OS_MINGW || AX_OS_CYGWIN
public:
	const int	kInvalidHandle = -1;
	int			nativeFileDescriptor()	{ return _fd; }
private:
	void		_os_open	( StrView filename, int access_flag );
	bool		_os_lock	( int flags );
	int			_fd = kInvalidHandle;
#endif

private:
	String		_filename;
	void		_check_fd	() const;
};

} // namespace 