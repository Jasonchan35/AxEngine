export module AxCore.IArray;

#include "AxBase.h"

import <algorithm>;
export import AxCore.BasicType;
export import AxCore.ByteOrder;
export import AxCore.Span;
export import AxCore.Math;

import AxCore.Allocator;
import AxCore.MemoryUtil;
import AxCore.RawArrayUtil;

export namespace ax {

template<class T>
class IArray : public NonCopyable {
protected:
	static constexpr bool kEnableSmallBufferOptimization = true;

	IArray(T* data, Int initCap) : _storage(data, initCap) {}
	virtual ~IArray() = default;
	
public:
	constexpr T*  data() { return _storage.data(); }
	constexpr Int size() const { return _storage.size(); }
	constexpr Int capacity() const { return _storage.capacity(); }

	constexpr void reserve(Int newCapacity);
	
	template<class... Args>
	constexpr void resize(Int newSize, Args&&... args);

	constexpr void append(const T& item);
	constexpr void append(T && item);

	constexpr void clear();
	constexpr void clearAndFree();
	
protected:
	virtual	MemoryBlock<T>	onMalloc(Int reqSize) = 0;
	virtual	void			onFree	(T* p) = 0;
	
private:
	struct NormalStorage {
		Int _capacity; // in big endian, so the first bit can be used to distinct SmallStorage or NormalStorage
		Int _size;
		T*  _data;
		static_assert(sizeof(Int) == sizeof(UInt));
		constexpr Int        capacity() const;
		constexpr void       setCapacity(Int v);
	};

	struct SmallStorage {
		u8 _capacity; // will be negative
		u8 _size;
		T  _data[0];
		static constexpr u8  kMask = 0x80;
		static constexpr Int kExtraCapacity = Math::max<Int>(0u, (AX_SIZE_OF(NormalStorage) - AX_ALIGN_OF(T)) / AX_SIZE_OF(T));
		constexpr Int        capacity() const { return kExtraCapacity + Int(_capacity & ~kMask); }
		constexpr void       setCapacity(Int v);
	};

	union Storage {
	public:
		static constexpr bool kEnableSmallBuffer = IArray::kEnableSmallBufferOptimization;

		SmallStorage  _small;
		NormalStorage _normal;

		constexpr Storage(T* data, Int initCap);
		constexpr ~Storage();
		
		constexpr bool isSmall() const;
		constexpr Int capacity() const;

		constexpr const T* data() const { return isSmall() ? _small._data : _normal._data; }
		constexpr T*       data()		{ return isSmall() ? _small._data : _normal._data; }
		constexpr Int      size() const { return isSmall() ? _small._size : _normal._size; }

		constexpr void setSize(Int v);
		constexpr void setData(T* data, Int cap);
	};

	Storage _storage;
	constexpr void _reserve(Int reqCapacity);
};

template <class T> AX_INLINE
constexpr void IArray<T>::reserve(Int newCapacity) {
	auto oldCap = capacity();
	if (newCapacity <= oldCap) return;
	_reserve(newCapacity);
}

template <class T> inline
constexpr void IArray<T>::_reserve(Int reqCapacity) {
	auto  memoryBlock  = onMalloc(reqCapacity);
	auto  newCapacity  = memoryBlock.size();  
	auto  oldSize = size();
	auto* oldData = data();
	auto* newData = reinterpret_cast<T*>(memoryBlock.data());
	if (newData != oldData) {
		if (oldData) {
			RawArrayUtil::moveConstructorAndDestructor(newData, oldData, oldSize);
			if (!_storage.isSmall()) {
				onFree(oldData);
			}
		}
		memoryBlock.detach();
	}
	_storage.setData(newData, newCapacity);
}

template <class T>
template <class ... Args> inline
constexpr void IArray<T>::resize(Int newSize, Args&&... args) {
	if (newSize < 0) throw Error_InvalidSize(AX_SRC_LOC);
	auto oldSize = size();
	auto* oldData = data();
	if (newSize == oldSize) return;
	if( newSize <  oldSize ) {
		auto dst = oldData + newSize;
		auto n   = oldSize  - newSize;
		RawArrayUtil::destructor(dst, n);
	}else{
		reserve(newSize);
		RawArrayUtil::constructor(data() + oldSize, newSize - oldSize, AX_FORWARD(args)...);
	}
	_storage.setSize(newSize);
}

template <class T>
constexpr void IArray<T>::append(const T& item) {
	auto oldSize = size();
	reserve(oldSize + 1);
	ax_call_constructor<T>(data() + oldSize, item);
	_storage.setSize(oldSize + 1);
}

template <class T>
constexpr void IArray<T>::append(T && item) {
	auto oldSize = size();
	reserve(oldSize + 1);
	ax_call_constructor<T>(data() + oldSize, AX_FORWARD(item));
	_storage.setSize(oldSize + 1);
}

template <class T> inline
constexpr void IArray<T>::clear() {
	RawArrayUtil::destructor(data(), size());
	_storage.setSize(0);
}

template <class T> inline
constexpr void IArray<T>::clearAndFree() {
	clear();
	auto* oldData = data();
	if (oldData) {
		onFree(oldData);
		_storage.setData(nullptr, 0);
	}	
}

template <class T> inline
constexpr Int IArray<T>::Storage::capacity() const {
	if constexpr (kEnableSmallBufferOptimization) {
		return isSmall() ? _small.capacity() : _normal.capacity();
	} else {
		return _normal.capacity();
	}
}

template <class T> inline
constexpr void IArray<T>::Storage::setSize(Int v) {
	if (isSmall()) {
		AX_ASSERT(v <= i8_max);
		_small._size = static_cast<i8>(v);
	} else {
		_normal._size = v;
	}
}

template <class T> inline
constexpr void IArray<T>::Storage::setData(T* data, Int cap) {
	auto oldSize = size();
	_normal._size = oldSize;
	_normal._data = data;
	_normal.setCapacity(cap);
}

template <class T>
constexpr void IArray<T>::SmallStorage::setCapacity(Int v) {
	if constexpr (kEnableSmallBufferOptimization) {
		if (v < 0 || v > i8_max) throw Error_InvalidSize(AX_SRC_LOC);
		_capacity = static_cast<u8>(v) | kMask;
	} else {
		AX_ASSERT(false);
	}
}

template <class T> inline
constexpr void IArray<T>::NormalStorage::setCapacity(Int v) {
	if (v < 0) throw Error_InvalidSize(AX_SRC_LOC);
	if constexpr (kEnableSmallBufferOptimization) {
		_capacity = ByteOrder::BigEndian::fromHost( v);
	} else {
		_capacity = v;
	}
}

template <class T> inline
constexpr Int IArray<T>::NormalStorage::capacity() const {
	if constexpr (kEnableSmallBufferOptimization) {
		return ByteOrder::BigEndian::toHost(_capacity);
	} else {
		return _capacity;
	}
}

template <class T> AX_INLINE
constexpr IArray<T>::Storage::Storage(T* data, Int initCap) {
	if (kEnableSmallBufferOptimization && initCap <= i8_max) {
		_small.setCapacity(initCap);
		_small._size = 0;
		// AX_ASSERT(_small._data == data);
	} else {
		_normal.setCapacity(initCap);
		_normal._size = 0;
		_normal._data = data;
	}
}

template <class T> inline
constexpr IArray<T>::Storage::~Storage() {
	if constexpr (kEnableSmallBufferOptimization) {
		AX_ASSERT_MSG(isSmall() || _normal._data == nullptr, "IArray data is not freed");
	} else {
		AX_ASSERT_MSG(_normal._data == nullptr, "IArray data is not freed");
	}
}

template <class T> inline
constexpr bool IArray<T>::Storage::isSmall() const {
	if constexpr (kEnableSmallBufferOptimization) {
		return (_small._capacity & SmallStorage::kMask) != 0;
	} else {
		return false;
	}
}
} // namespace
