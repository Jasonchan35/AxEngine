export module AxCore.IArrayStorage;

#include "AxBase.h"

import <algorithm>;
export import AxCore.BasicType;
export import AxCore.ByteOrder;
export import AxCore.Span;
export import AxCore.Math;
export import AxCore.Span;

export import AxCore.Allocator;
export import AxCore.MemoryUtil;

export namespace ax {

template<class T>
class IArrayStorage : public NonCopyable {
protected:
	static constexpr bool kSmallBufferOptimization = true;

	IArrayStorage(T* data, Int initCap) : _storage(data, initCap) {}
	virtual ~IArrayStorage() = default;

protected:
	virtual	MemoryBlock<T>	onStorageMalloc(Int reqSize) = 0;
	virtual	void			onStorageFree	(T* p) = 0;

	constexpr bool _storageReserve(Int newCapacity);
	constexpr void _storageRreserveImpl(Int reqCapacity);
	
	template<class... Args>
	constexpr void _storageResize(Int newSize, Args&&... args);

	constexpr void _storageClear();
	constexpr void _storageClearAndFree();
	
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
		static constexpr Int kExtraCapacity = Math::max<Int>(0u, (ax_sizeof<NormalStorage> - ax_alignof<T>) / ax_sizeof<T>);
		constexpr Int        capacity() const { return kExtraCapacity + Int(_capacity & ~kMask); }
		constexpr void       setCapacity(Int v);
	};

	union Storage {
	public:
		static constexpr bool kSmallBufferOptimization = IArrayStorage::kSmallBufferOptimization;

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
};

template <class T> AX_INLINE
constexpr bool IArrayStorage<T>::_storageReserve(Int newCapacity) {
	auto oldCap = _storage.capacity();
	if (newCapacity <= oldCap) return false;
	_storageRreserveImpl(newCapacity);
	return true;
}

template <class T> inline
constexpr void IArrayStorage<T>::_storageRreserveImpl(Int reqCapacity) {
	auto  memoryBlock  = onStorageMalloc(reqCapacity);
	auto  newCapacity  = memoryBlock.size();
	if (newCapacity < reqCapacity) throw Error_Allocator();
	
	auto  oldSize = _storage.size();
	auto* oldData = _storage.data();
	auto* newData = reinterpret_cast<T*>(memoryBlock.data());
	if (newData != oldData) {
		if (oldData) {
			MemoryUtil::moveConstructorAndDestructor(newData, oldData, oldSize);
			if (!_storage.isSmall()) {
				onStorageFree(oldData);
			}
		}
		memoryBlock.detach();
	}
	
	_storage.setData(newData, newCapacity);
}

template <class T>
template <class ... Args> inline
constexpr void IArrayStorage<T>::_storageResize(Int newSize, Args&&... args) {
	if (newSize < 0) throw Error_InvalidSize();
	auto oldSize = _storage.size();
	auto* oldData = _storage.data();
	if (newSize == oldSize) return;
	if( newSize <  oldSize ) {
		auto dst = oldData + newSize;
		auto n   = oldSize  - newSize;
		MemoryUtil::destructor(dst, n);
	}else{
		_storageReserve(newSize);
		auto* newData = _storage.data();
		MemoryUtil::constructor(newData + oldSize, newSize - oldSize, AX_FORWARD(args)...);
	}
	_storage.setSize(newSize);
}

template <class T> AX_INLINE
constexpr void IArrayStorage<T>::_storageClear() {
	MemoryUtil::destructor(_storage.data(), _storage.size());
	_storage.setSize(0);
}

template <class T> inline
constexpr void IArrayStorage<T>::_storageClearAndFree() {
	_storageClear();
	if (auto* oldData = _storage.data()) {
		if (!_storage.isSmall()) {
			onStorageFree(oldData);
		}
		_storage.setData(nullptr, 0);
	}
}

template <class T> AX_INLINE
constexpr Int IArrayStorage<T>::Storage::capacity() const {
	if constexpr (kSmallBufferOptimization) {
		return isSmall() ? _small.capacity() : _normal.capacity();
	} else {
		return _normal.capacity();
	}
}

template <class T> AX_INLINE
constexpr void IArrayStorage<T>::Storage::setSize(Int v) {
	if (isSmall()) {
		AX_ASSERT(v <= i8_max);
		_small._size = static_cast<i8>(v);
	} else {
		_normal._size = v;
	}
}

template <class T> AX_INLINE
constexpr void IArrayStorage<T>::Storage::setData(T* data, Int cap) {
	auto oldSize = size();
	_normal._size = oldSize;
	_normal._data = data;
	_normal.setCapacity(cap);
}

template <class T> AX_INLINE
constexpr void IArrayStorage<T>::SmallStorage::setCapacity(Int v) {
	if constexpr (kSmallBufferOptimization) {
		if (v < 0 || v > i8_max) throw Error_InvalidSize();
		_capacity = static_cast<u8>(v) | kMask;
	} else {
		AX_ASSERT(false);
	}
}

template <class T> AX_INLINE
constexpr void IArrayStorage<T>::NormalStorage::setCapacity(Int v) {
	if (v < 0) throw Error_InvalidSize();
	if constexpr (kSmallBufferOptimization) {
		_capacity = ByteOrder::BigEndian::fromHost( v);
	} else {
		_capacity = v;
	}
}

template <class T> AX_INLINE
constexpr Int IArrayStorage<T>::NormalStorage::capacity() const {
	if constexpr (kSmallBufferOptimization) {
		return ByteOrder::BigEndian::toHost(_capacity);
	} else {
		return _capacity;
	}
}

template <class T> AX_INLINE
constexpr IArrayStorage<T>::Storage::Storage(T* data, Int initCap) {
	if (kSmallBufferOptimization && initCap <= i8_max) {
		_small.setCapacity(initCap);
		_small._size = 0;
		// AX_ASSERT(_small._data == data);
	} else {
		_normal.setCapacity(initCap);
		_normal._size = 0;
		_normal._data = data;
	}
}

template <class T> AX_INLINE
constexpr IArrayStorage<T>::Storage::~Storage() {
	if constexpr (kSmallBufferOptimization) {
		AX_ASSERT_MSG(isSmall() || _normal._data == nullptr, "IArray data is not freed");
	} else {
		AX_ASSERT_MSG(_normal._data == nullptr, "IArray data is not freed");
	}
}

template <class T> AX_INLINE
constexpr bool IArrayStorage<T>::Storage::isSmall() const {
	if constexpr (kSmallBufferOptimization) {
		return (_small._capacity & SmallStorage::kMask) != 0;
	} else {
		return false;
	}
}
} // namespace
