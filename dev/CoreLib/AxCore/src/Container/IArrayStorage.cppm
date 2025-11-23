export module AxCore.IArrayStorage;

#include "AxBase.h"

import <algorithm>;
import <bit>;
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

	constexpr IArrayStorage(T* data, Int initCap) : _storage(data, initCap) {}
	constexpr virtual ~IArrayStorage() = default;

protected:
	constexpr virtual MemAllocResult<T>	onStorageLocalBuf() = 0;
	constexpr virtual	MemAllocResult<T>	onStorageMalloc(Int reqSize) = 0;
	constexpr virtual	void			onStorageFree(T* p) = 0;

	constexpr void _storageMove(IArrayStorage<T>&& rhs);
	constexpr bool _storageReserve(Int newCapacity);
	constexpr void _storageRreserveImpl(Int reqCapacity);
	
	template<class... Args>
	constexpr void _storageResize(Int newSize, Args&&... args);

	constexpr void _storageClear();
	constexpr void _storageClearAndFree();
	constexpr void _storageResetToLocalBuf();

	
	struct NormalStorage {
		u64                  _isSmall         : 1; // share between normal and small storage
		u64                  _capacity        : 63;
		u64                  _isAllocatedData : 1;
		u64                  _size            : 63;
		T*                   _data;
		static constexpr u64 kCapacityMax = u64_max >> 1;
		constexpr void       setCapacity(Int v);
	};

	struct SmallStorage {
		u16                  _isSmall  : 1; // share between normal and small storage
		u16                  _capacity : 15;
		u16                  _size;
		T                    _data[0];
		static constexpr u16 kCapacityMax = u16_max >> 1;
		constexpr void       setCapacity(Int v);
	};
	
	union Storage {
	public:
		SmallStorage  _small;
		NormalStorage _normal;

		static constexpr bool kSmallBufferOptimization = IArrayStorage::kSmallBufferOptimization;
		static constexpr Int  kSmallDataOffset    = offsetof(SmallStorage, _data);
		static constexpr Int  kSmallExtraCapacity = Math::max0<Int>(ax_sizeof<NormalStorage> - kSmallDataOffset) / ax_sizeof<T>;


		constexpr Storage(T* data, Int initCap) { resetToLocalBuf(data, initCap); }
		constexpr ~Storage();

		constexpr void resetToLocalBuf(T* data, Int initCap);
		
		constexpr bool isSmall() const { return _small._isSmall; }
		constexpr bool isAllocatedData() const { return !isSmall() && _normal._isAllocatedData; }
		constexpr Int capacity() const;

		constexpr const T* data() const { return isSmall() ? _small._data : _normal._data; }
		constexpr T*       data()		{ return isSmall() ? _small._data : _normal._data; }
		constexpr Int      size() const { return isSmall() ? _small._size : _normal._size; }

		constexpr void setSize(Int v);
		constexpr void setAllocDataPtr(T* data, Int cap);
	};

	Storage _storage;
};

template< class T > inline
constexpr void IArrayStorage<T>::_storageMove(IArrayStorage<T> && rhs) {
	if (this == &rhs) { AX_ASSERT(false); return; }

	auto srcData = rhs._storage.data();
	auto srcSize = rhs._storage.size();
	
	if (rhs._storage.isAllocatedData()) { // move buffer if is allocated data
		_storageClearAndFree();
		_storage.setAllocDataPtr(rhs._storage.data(), rhs._storage.capacity());
		_storage.setSize(srcSize);
		rhs._storageResetToLocalBuf();
	} else {
		_storageClear();
		_storageReserve(srcSize);
		MemUtil::moveConstructorAndDestructor(_storage.data(), srcData, srcSize);
		_storage.setSize(srcSize);
		rhs._storage.setSize(0);
	}
}


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
	auto  newCapacity  = memoryBlock.size;
	if (newCapacity < reqCapacity) throw Error_Allocator();
	
	auto  oldSize = _storage.size();
	auto* oldData = _storage.data();
	auto* newData = reinterpret_cast<T*>(memoryBlock.data);
	if (newData != oldData) {
		if (oldData) {
			MemUtil::moveConstructorAndDestructor(newData, oldData, oldSize);
			if (!_storage.isSmall()) {
				onStorageFree(oldData);
			}
		}
		memoryBlock.detach();
	}
	
	_storage.setAllocDataPtr(newData, newCapacity);
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
		MemUtil::destructor(dst, n);
	}else{
		_storageReserve(newSize);
		auto* newData = _storage.data();
		MemUtil::constructor(newData + oldSize, newSize - oldSize, AX_FORWARD(args)...);
	}
	_storage.setSize(newSize);
}

template <class T> AX_INLINE
constexpr void IArrayStorage<T>::_storageClear() {
	MemUtil::destructor(_storage.data(), _storage.size());
	_storage.setSize(0);
}

template <class T> inline
constexpr void IArrayStorage<T>::_storageClearAndFree() {
	_storageClear();
	if (_storage.isAllocatedData()) {
		onStorageFree(_storage.data());
		_storageResetToLocalBuf();
	}
}

template <class T>
constexpr void IArrayStorage<T>::_storageResetToLocalBuf() {
	auto block = onStorageLocalBuf();
	if (block.allocator) throw Error_Allocator();
	_storage.resetToLocalBuf(block.data, block.size);
}

template <class T> AX_INLINE
constexpr Int IArrayStorage<T>::Storage::capacity() const {
	if constexpr (kSmallBufferOptimization) {
		return isSmall() ? _small._capacity : _normal._capacity;
	} else {
		return _normal._capacity;
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
constexpr void IArrayStorage<T>::Storage::setAllocDataPtr(T* data, Int cap) {
	auto oldSize = size();
	_normal._size = oldSize;
	_normal._data = data;
	_normal._isAllocatedData = true;
	_normal.setCapacity(cap);
}

template <class T> AX_INLINE
constexpr void IArrayStorage<T>::SmallStorage::setCapacity(Int v) {
	if constexpr (kSmallBufferOptimization) {
		if (v < 0 || v + Storage::kSmallExtraCapacity > kCapacityMax) throw Error_InvalidSize();
		_isSmall = true;
		_capacity = static_cast<u16>(v + Storage::kSmallExtraCapacity);
	} else {
		AX_ASSERT(false);
	}
}

template <class T> AX_INLINE
constexpr void IArrayStorage<T>::NormalStorage::setCapacity(Int v) {
	if (v < 0 || v >= kCapacityMax) throw Error_InvalidSize();
	_isSmall = false;
	_capacity = v;
}

template <class T> AX_INLINE
constexpr void IArrayStorage<T>::Storage::resetToLocalBuf(T* data, Int initCap) {
	if (kSmallBufferOptimization && initCap <= SmallStorage::kCapacityMax) {
		_small.setCapacity(initCap);
		_small._size = 0;
	} else {
		_normal.setCapacity(initCap);
		_normal._size = 0;
		_normal._isAllocatedData = false;
		_normal._data = data;
	}
}

template <class T> AX_INLINE
constexpr IArrayStorage<T>::Storage::~Storage() {
	AX_ASSERT_MSG(!isAllocatedData(), " Allocated data not freed yet, please call _storageClearAndFree in destructor");
}

} // namespace
