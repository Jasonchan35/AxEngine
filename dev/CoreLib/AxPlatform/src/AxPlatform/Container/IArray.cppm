module;
#include "AxPlatform-pch.h"

export module AxPlatform.IArray;
export import AxPlatform.IArrayStorage;

export namespace ax {

template<class T>
class IArray : public IArrayStorage<T> {
	using Base = IArrayStorage<T>;
	using Base::_storage;
protected:
	constexpr IArray(T* data, Int initCap) : Base(data, initCap) {}
	using MSpan = MutSpan<T>;
	using CSpan =    Span<T>;
public:
	constexpr T*  data() { return _storage.data(); }
	constexpr Int size() const { return _storage.size(); }
	constexpr Int capacity() const { return _storage.capacity(); }

	constexpr void clear() { Base::_storageClear(); }
	constexpr void clearAndFree() { Base::_storageClearAndFree(); }
	
	constexpr void reserve(Int newCapacity) { Base::_storageReserve(newCapacity); }
	
	template<class... Args>
	constexpr void resize(Int newSize, Args&&... args) { Base::_storageResize(newSize, AX_FORWARD(args)...); }

	constexpr void append(const T& item);
	constexpr void append(T && item);

	constexpr void operator << (const T &  item)  { append(item); }
	constexpr void operator << (      T && item)  { append(AX_FORWARD(item)); }

	MSpan	span		()			{ return MutSpan<T>(data(), size()); }
	CSpan	span		() const	{ return    Span<T>(data(), size()); }
	CSpan	constSpan	() const	{ return    Span<T>(data(), size()); }

	operator  MutSpan<T>	()			{ return span(); }
	operator  Span<T>		() const	{ return span(); }

			T & at(Int i)				{ return span().at(i); }
	const	T & at(Int i) const 		{ return span().at(i); }

			T & unsafe_at(Int i)		{ return span().unsafe_at(i); }
	const	T & unsafe_at(Int i) const	{ return span().unsafe_at(i); }
	
			T & operator[](Int i)		{ return span()[i]; }
	const	T & operator[](Int i) const { return span()[i]; }
	
	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()			{ return data(); }
	constexpr CIter	begin	() const	{ return data(); }
	constexpr  Iter	end		()			{ return data() + size(); }
	constexpr CIter	end		() const	{ return data() + size(); }
};

template<class T> struct Type_IsIArray_Struct : std::false_type {};
template<class T> struct Type_IsIArray_Struct< IArray<T> > : std::true_type {};
template<class T> constexpr bool Type_IsArray = Type_IsIArray_Struct<T>::value; 


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
