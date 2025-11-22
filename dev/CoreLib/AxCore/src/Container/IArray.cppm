export module AxCore.IArray;

#include "AxBase.h"

export import AxCore.IArrayStorage;

export namespace ax {

template<class T>
class IArray : public IArrayStorage<T> {
	using Base = IArrayStorage<T>;
	using Base::_storage;
protected:
	constexpr IArray(T* data, Int initCap) : Base(data, initCap) {}
	
public:
	constexpr T*  data() { return _storage.data(); }
	constexpr Int size() const { return _storage.size(); }
	constexpr Int capacity() const { return _storage.capacity(); }

	constexpr MutSpan<T> toMutSpan() { return MutSpan<T>(data(), size()); }
	constexpr operator MutSpan<T>()  { return toMutSpan(); }

	constexpr void clear() { Base::_storageClear(); }
	constexpr void clearAndFree() { Base::_storageClearAndFree(); }
	
	constexpr void reserve(Int newCapacity) { Base::_storageReserve(newCapacity); }
	
	template<class... Args>
	constexpr void resize(Int newSize, Args&&... args) { Base::_storageResize(newSize, AX_FORWARD(args)...); }

	constexpr void append(const T& item);
	constexpr void append(T && item);
	
	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()			{ return data(); }
	constexpr CIter	begin	() const	{ return data(); }
	constexpr  Iter	end		()			{ return data() + size(); }
	constexpr CIter	end		() const	{ return data() + size(); }
};

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

} // namespace
