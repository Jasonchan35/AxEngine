module;

module AxCore.FileStream;
import AxCore.Logger;

namespace  ax {

void FileStream::operator=(FileStream && r) {
	close();
	_fd = r._fd;
	_filename = std::move(r._filename);

	r._fd = kInvalidHandle;
	r._filename.clear();
}

FileStream::~FileStream() {
	close();
}

void FileStream::_check_fd() const {
	if( ! isOpened() ) {
		throw Error_File();
	}
}

void FileStream::openRead(StrView filename) {
	open(filename, FileMode::OpenExists, FileAccess::Read, FileShareMode::Read);
}

void FileStream::openAppend(StrView filename) {
	open(filename, FileMode::OpenOrCreate, FileAccess::ReadWrite, FileShareMode::Read);
	setPosEnd(0);
}

void FileStream::openWrite(StrView filename, bool truncate) {
	open(filename, FileMode::OpenOrCreate, FileAccess::ReadWrite, FileShareMode::Read);
	if (truncate) {
		setFileSize(0);
	}
}

void FileStream::appendReadAllUtf8 ( IStringA& buf ) {
	_check_fd();

	FileSize cur = getPos();
	FileSize fileSize = getFileSize();

	if( cur > fileSize ) throw Error_File();

	Int oldSize = buf.size();
	Int dataSize = SafeCast(fileSize - cur);
	Int newSize = oldSize + dataSize; 

	try{
		buf.resize(newSize);
		auto mutByteSpan = buf.toMutByteSpan();
		readBytes(mutByteSpan.slice(oldSize, dataSize));

	}catch(...) {
		buf.clear();
		throw;
	}
}

void FileStream::appendReadAllText ( IString & out_buf ) {
	#if AX_TYPE_CHAR_DEFINE == 'A'
		appendReadAllUtf8( out_buf );
	#else
		TempStringA tmp;
		appendReadAllUtf8( tmp );
		out_buf.appendUtf( tmp );
	#endif
}

void FileStream::writeText ( StrView buf ) {
	TempStringA bufA;
	bufA.setUtf(buf);
	writeUtf8(bufA);
}

void FileStream::writeUtf8 ( const StrViewA& buf ) {
	writeBytes( buf.toByteSpan() );
}

void FileStream::appendReadAllBytes ( IByteArray & buf ) {
	FileSize cur = getPos();

	FileSize	file_size = getFileSize();

	if( cur > file_size ) throw Error_File();

	auto old_size  = buf.size();
	Int data_size = SafeCast(file_size - cur);

	buf.incSize(data_size);
	try{
		readBytes(buf.span().slice(old_size, data_size));
	}catch(...){
		buf.clear();
		throw;
	}
}

#if 0
#pragma mark ================= Unix ====================
#endif
#if AX_OS_UNIX || AX_OS_MINGW || AX_OS_CYGWIN

FileStream::FileStream() {
	_fd = -1;
}

bool FileStream::isOpened() const {
	return _fd != -1;
}

void FileStream::close() {
	if( isOpened() ) {
		::close( _fd );
		_fd = -1;
	}
}

void FileStream::flush () {
	int b = ::fsync( _fd );
	if( b != 0 ) throw Error_Undefined();
}

UtcTime	FileStream::lastAccessTime 	() {
	_check_fd();
	struct stat info;
	if( 0 != fstat( _fd, &info ) ) throw Error_Undefined();
	UtcTime t;

    #if AX_OS_MACOSX || AX_OS_IOS
    	t = UtcTime_make(info.st_atimespec);
	#elif AX_OS_UNIX || AX_OS_MINGW || AX_OS_CYGWIN
        t = UtcTime_make(info.st_atime);
    #endif
	return t;
}

UtcTime	FileStream::lastWriteTime	( ) {
	_check_fd();
	struct stat info;
	if( 0 != fstat( _fd, &info ) ) throw Error_Undefined();
	UtcTime t;

	#if AX_OS_MACOSX || AX_OS_IOS
		t = UtcTime_make(info.st_mtimespec);
	#elif AX_OS_MINGW
		t = UtcTime_make(info.st_mtime);
	#else
		t = UtcTime_make(info.st_mtim);
	#endif

	return t;
}

void FileStream::setLastWriteTime(const UtcTime& t) {
	_check_fd();

	#if AX_OS_MACOSX || AX_OS_IOS
		auto s = timeval_make(t);
		timeval arr[2] = {s,s};
		futimes(_fd, arr);
	#elif AX_OS_ANDROID
		auto s = timespec_make(t);
		timespec arr[2] = {s,s};
		utimensat(_fd, nullptr, arr, 0);
	#else
		auto s = timespec_make(t);
		timespec arr[2] = {s,s};
		futimens(_fd, arr);
	#endif
}

void FileStream::setPos( FileSize  n ) {
	_check_fd();
	off_t ret = lseek( _fd, n, SEEK_SET );
	if( ret == -1 ) throw Error_Undefined();
}

void FileStream::advPos( FileSize  n ) {
	_check_fd();
	off_t ret = lseek( _fd, n, SEEK_CUR );
	if( ret == -1 ) throw Error_Undefined();
}

void FileStream::setPosEnd( FileSize  n ) {
	_check_fd();
	off_t ret = lseek( _fd, n, SEEK_END );
	if( ret == -1 ) throw Error_Undefined();
}

FileSize FileStream::getPos () {
	_check_fd();
	FileSize n = 0;
	off_t ret = lseek( _fd, n, SEEK_SET );
	if( ret == -1 ) throw Error_Undefined();
	return FileSize( ret );
}

FileSize FileStream::getFileSize() {
	_check_fd();
	FileSize out = 0;

	off_t curr, r;

	curr = lseek( _fd, 0, SEEK_CUR );	if( curr == -1 ) throw Error_Undefined();
	r = lseek( _fd, 0, SEEK_END );		if( r == -1 ) throw Error_Undefined();
	out = r;
	r = lseek( _fd, curr, SEEK_SET );	if( r == -1 ) throw Error_Undefined();

	return FileSize( out );
}

void FileStream::setFileSize( FileSize newSize ) {
	_check_fd();

	FileSize	oldPos = getPos();

	off_t o = newSize;
	if( 0 != ftruncate( _fd, o ) ) throw Error_Undefined();

	if( oldPos < newSize ) setPos( oldPos );
}

void FileStream::readBytes ( Byte* buf, Int sizeInBytes ) {
	_check_fd();

	if( sizeInBytes <= 0 ) return;
	auto ret = ::read( _fd, buf, (size_t)sizeInBytes );
	if( ret <= 0 ) throw Error_Undefined();
}

void FileStream::writeBytes( ByteSpan buf ) {
	_check_fd();
	if( buf.size() <= 0 ) return;
	auto ret = ::write( _fd, buf.data(), (size_t)buf.sizeInBytes() );
	if( ret <= 0 ) throw Error_Undefined();
}

//----- open file
void FileStream::_os_open( StrView filename, int access_flag ) {
	close();
#if axOS_Linux
	access_flag |= O_LARGEFILE;
#endif
	auto filenameA = TempStringA::s_utf(filename);

	#if AX_OS_MINGW
		mode_t mode = S_IRUSR | S_IWUSR;
	#else
		mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	#endif

	_fd = ::open( filenameA.c_str(), access_flag, mode );
	if( _fd == -1 ) {
		AX_LOG("open file error {}: filename=[{}] cwd=[{}]", errno, filename, FilePath::currentDir());
		switch( errno ) {
			case EACCES: throw Error_Undefined();
			case EEXIST: throw Error_Undefined();
			case ENFILE: throw Error_Undefined();
			case EMFILE: throw Error_Undefined();
			case ENOENT: throw Error_Undefined();
			default:
				AX_ASSERT(false);
				throw Error_Undefined();
				break;
		}
	}
}

void FileStream::open ( StrView filename, FileMode mode, FileAccess access, FileShareMode share ) {
	_filename = filename;
	int access_flag = 0;

	switch( mode ) {
		case FileMode::CreateNew: 	access_flag |= O_CREAT | O_EXCL;	break;
		case FileMode::OpenExists:	break;
		case FileMode::OpenOrCreate: 	access_flag |= O_CREAT;				break;
	}

	switch( access ) {
		case FileAccess::Read:		access_flag |= O_RDONLY;	break;
		case FileAccess::ReadWrite:	access_flag |= O_RDWR;		break;
		case FileAccess::WriteOnly:	access_flag |= O_WRONLY;	break;
	}

	_os_open( filename, access_flag );
}

bool	FileStream::_os_lock( int flags ) {
	_check_fd();

	int rc;
	/* keep trying if flock() gets interrupted (by a signal) */
	while ((rc = flock( _fd, flags)) < 0 && errno == EINTR)
		continue;

	if (rc == -1) return false;
	return true;
}

void	FileStream::lock( bool exclusive ) {
	if( exclusive ) {
		if( ! _os_lock( LOCK_EX ) ) throw Error_Undefined();
	}else{
		if( ! _os_lock( LOCK_SH ) ) throw Error_Undefined();
	}
}

bool	FileStream::trylock( bool exclusive ) {
	if( exclusive ) {
		return _os_lock( LOCK_EX | LOCK_NB );
	}else{
		return _os_lock( LOCK_SH | LOCK_NB );
	}
}

void	FileStream::unlock() {
	_os_lock( LOCK_UN );
}

#endif //axOS_Unix

#if 0
#pragma mark ================= Windows ====================
#endif
#if AX_OS_WINDOWS

FileStream::FileStream() {
	_fd = INVALID_HANDLE_VALUE;
}

bool FileStream::isOpened() const {
	return _fd != INVALID_HANDLE_VALUE;
}

void FileStream::close() {
	if( isOpened() ) {
		BOOL ret = ::CloseHandle( _fd );
		AX_UNUSED(ret);
		assert( ret );
		_fd = INVALID_HANDLE_VALUE;
	}
}

void FileStream::flush () {
	BOOL b = FlushFileBuffers( _fd );
	if( ! b ) throw Error_Undefined();
}

UtcTime	FileStream::lastAccessTime 	() {
	_check_fd();
	BY_HANDLE_FILE_INFORMATION info;
	if( ! ::GetFileInformationByHandle( _fd, &info ) ) throw Error_Undefined();
	auto t = UtcTime_make(info.ftLastAccessTime);
	return t;
}

UtcTime FileStream::lastWriteTime	() {
	_check_fd();

	BY_HANDLE_FILE_INFORMATION info;
	if( ! ::GetFileInformationByHandle( _fd, &info ) ) throw Error_Undefined();
	auto t = UtcTime_make(info.ftLastWriteTime);
	return t;
}

void FileStream::setLastWriteTime(const UtcTime& t) {
	_check_fd();
	auto ft = FILETIME_make(t);
	SetFileTime(_fd, nullptr, nullptr, &ft);
}

FileSize FileStream::getPos() {
	_check_fd();

	LONG  hi = 0;
	DWORD low = SetFilePointer( _fd, 0, &hi, FILE_CURRENT );
	i64	value = low | ( static_cast<i64>(hi) << 32 );
	return SafeCast(value);
}

void FileStream::setPos( FileSize n ) {
	_check_fd();

	i64 tmp = SafeCast(n);
	LONG hi = tmp >> 32;
	SetFilePointer( _fd, static_cast<LONG>(tmp), &hi, FILE_BEGIN );
}

void FileStream::advPos( FileSize n ) {
	_check_fd();

	i64 tmp = SafeCast(n);
	LONG hi = tmp >> 32;
	SetFilePointer( _fd, static_cast<LONG>(tmp), &hi, FILE_CURRENT );
}

void FileStream::setPosEnd(FileSize n) {
	_check_fd();

	i64 tmp = SafeCast(n);
	LONG hi = tmp >> 32;
	SetFilePointer( _fd, static_cast<LONG>(tmp), &hi, FILE_END );
}

FileSize FileStream::getFileSize() {
	_check_fd();

	DWORD high = 0;
	DWORD low = ::GetFileSize( _fd, &high );
	if( low == INVALID_FILE_SIZE ) throw Error_Undefined();

	u64 tmp = (static_cast<u64>(high) << 32) | low;
	return SafeCast(tmp);
}

void	FileStream::setFileSize		( FileSize newSize ) {
	_check_fd();

	FileSize	oldPos = getPos();
	setPos( newSize );
	SetEndOfFile( _fd );

	if( oldPos < newSize ) setPos( oldPos );
}

void FileStream::readBytes(MutByteSpan buf) {
	_check_fd();

	if (buf.size() <= 0 ) return;
	DWORD	n = SafeCast(buf.size());
	DWORD	result = 0;
	BOOL ret = ::ReadFile( _fd, buf.data(), n, &result, nullptr );
	if( !ret ) {
		DWORD e = GetLastError();
//		ax_log_win32_error("FileStream read file", e);
		switch( e ) {
			case ERROR_LOCK_VIOLATION: throw Error_Undefined();
		}
		throw Error_Undefined();
	}
}

void FileStream::writeBytes(ByteSpan buf) {
	_check_fd();

	if( buf.size() <= 0 ) return;

	DWORD	n = SafeCast(buf.sizeInBytes());
	DWORD	result = 0;
	BOOL ret = ::WriteFile( _fd, buf.data(), n, &result, nullptr );
	if( !ret ) {
		throw Error_Undefined();
	}
}

void FileStream::open	( StrView filename, FileMode mode, FileAccess access, FileShareMode share ) {
	_filename = filename;
	close();

	DWORD	create_flag	= 0;
	DWORD	access_flag = 0;
	DWORD	share_flag	= 0;

	switch( mode ) {
		case FileMode::CreateNew: 		create_flag |= CREATE_NEW;		break;
		case FileMode::OpenExists:		create_flag |= OPEN_EXISTING;	break;
		case FileMode::OpenOrCreate:	create_flag |= OPEN_ALWAYS;		break;
	}

	switch( access ) {
		case FileAccess::Read:			access_flag |= GENERIC_READ;					break;
		case FileAccess::ReadWrite:		access_flag |= GENERIC_READ | GENERIC_WRITE;	break;
		case FileAccess::WriteOnly:		access_flag |= GENERIC_WRITE;					break;
	}

	switch( share ) {
	    case FileShareMode::None: break;
		case FileShareMode::Read:		share_flag |= FILE_SHARE_READ;						break;
		case FileShareMode::ReadWrite:	share_flag |= FILE_SHARE_READ | FILE_SHARE_WRITE;	break;
		case FileShareMode::Write:		share_flag |= FILE_SHARE_WRITE;						break;
	}

	TempStringW filenameW;
	filenameW.setUtf(filename);

	_fd = ::CreateFile( filenameW.c_str(), access_flag, share_flag, nullptr, create_flag, FILE_ATTRIBUTE_NORMAL, nullptr );
	if( _fd == INVALID_HANDLE_VALUE ) {
		DWORD err = GetLastError();
//		ax_log_win32_error( "File_open", err );
		AX_LOG("open file error {}: filename=[{}] cwd=[{}]", errno, filename, FilePath::currentDir());
		switch( err ) {
			case ERROR_FILE_NOT_FOUND:		throw Error_File(Fmt("File not found: {}",			filename));
			case ERROR_PATH_NOT_FOUND:		throw Error_File(Fmt("File path not found: {}", 	filename));
			case ERROR_FILE_EXISTS:			throw Error_File(Fmt("File exists: {}",				filename));
			case ERROR_ALREADY_EXISTS:		throw Error_File(Fmt("File already exists: {}", 	filename));
			case ERROR_ACCESS_DENIED:		throw Error_File(Fmt("File access denied: {}",		filename));
			case ERROR_SHARING_VIOLATION:	throw Error_File(Fmt("File sharing violation: {}",	filename));
		}
		throw Error_Undefined();
	}
}

bool FileStream::_os_lock( DWORD flags ) {
	_check_fd();

    OVERLAPPED offset = {};
	const DWORD len = 0xffffffff;
	return 0 != ::LockFileEx( _fd, flags, 0, len, len, &offset );
}

void FileStream::lock( bool exclusive ) {
	if( exclusive ) {
		if( ! _os_lock( LOCKFILE_EXCLUSIVE_LOCK ) ) throw Error_Undefined();
	}else{
		if( ! _os_lock( 0 ) ) throw Error_Undefined();
	}
}

bool FileStream::trylock( bool exclusive ) {
	if( exclusive ) {
	    return _os_lock( LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY );
	}else{
	    return _os_lock( LOCKFILE_FAIL_IMMEDIATELY );
	}
}

void FileStream::unlock() {
	_check_fd();

    DWORD len = 0xffffffff;
	OVERLAPPED offset = {};
	if ( ! ::UnlockFileEx( _fd, 0, len, len, &offset ) ) {
		throw Error_Undefined();
	}
}

#endif //AX_OS_WINDOWS


} // namespace 