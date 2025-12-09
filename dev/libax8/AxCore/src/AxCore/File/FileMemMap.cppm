module;
#include "AxCore-pch.h"

export module AxCore.FileMemMap;
export import AxCore.FileStream;

export namespace ax {


class FileMemMap : public NonCopyable {
public:
	FileMemMap() = default;
	FileMemMap(StrView filename) { openFile(filename); }

	FileMemMap(FileMemMap && r) { operator=(AX_FORWARD(r)); }

	~FileMemMap();

	void openFile(StrView filename);
	void close();

	const	Byte*	data()	{ return _span.data(); }
	Int		size()	{ return _span.size(); }
	Int		sizeInBytes() { return _span.sizeInBytes(); }

	ByteSpan		span() { return _span; }
	operator	ByteSpan()  { return _span; }

	StrViewA	strViewA() { return StrViewA(reinterpret_cast<const CharA*>(span().data()), span().size()); }

	StrView		filename() const { return _file.filename(); }

	void operator=(FileMemMap && r);

private:
	FileStream	_file;

#if AX_OS_WINDOWS
	HANDLE	_mapping = nullptr;
#endif
	ByteSpan _span;
};

//-----------

inline
void FileMemMap::operator=(FileMemMap&& r) {
	close();
	_file = std::move(r._file);
#if AX_OS_WINDOWS
	_mapping   = r._mapping;
	r._mapping = nullptr;
#endif
}

class MutMemMapping {
public:

};

//------------------------
inline FileMemMap::~FileMemMap() {
	close();
}

#if AX_OS_WINDOWS

inline void FileMemMap::openFile(StrView filename) {
	close();

	_file.openRead(filename);
	auto size = _file.getFileSize();
	if (size <= 0) return;

	_mapping = CreateFileMapping(_file.nativeFileDescriptor(), nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (!_mapping) {
		throw Error_File();
	}

	auto* data = reinterpret_cast<Byte*>(MapViewOfFile(_mapping, FILE_MAP_READ, 0, 0, 0));
	if (!data) {
		throw Error_File();
	}

	_span.setPtr(data, size);
}

inline void FileMemMap::close() {
	if (_span.data()) {
		UnmapViewOfFile(_span.data());
		_span.setPtr(nullptr, 0);
	}

	if (_mapping) {
		CloseHandle(_mapping);
		_mapping = nullptr;
	}

	_file.close();
}

#else

inline void FileMemMap::openFile(StrView filename) {
	close();
	_file.openRead(filename);
	auto size = _file.getFileSize();
	if (size <= 0) return;

	size_t nativeSize = SafeCast(size);

	auto* data = reinterpret_cast<u8*>(mmap(0, nativeSize, PROT_READ, MAP_PRIVATE, _file.nativeFileDescriptor() , 0));
	if ( data == MAP_FAILED || data == nullptr ) {
		throw Error_Undefined();
	}

	_span.setPtr(data, size);
}

inline void FileMemMap::close() {
	if (_span.data()) {
		size_t nativeSize = SafeCast(_span.size());
		
		void* data = ax_const_cast(_span.data());
		munmap(data, nativeSize);
		_span.setPtr(nullptr, 0);
	}
	_file.close();
}

#endif

} // namespace 