export module AxCore.Span;

#include "AxBase.h"

export import AxCore.BasicType;
export import AxCore.Range;

import AxCore.MemoryUtil;

export namespace ax {

template<class T> class MutSpan;
template<class T> using Span = MutSpan<const T>;

using    ByteSpan	=    Span<Byte>;
using MutByteSpan	= MutSpan<Byte>;

using    IntSpan	=    Span<Int>;
using MutIntSpan	= MutSpan<Int>;

template<class T>
class MutSpan { // copyable
	using This = MutSpan;
	using CSpan = MutSpan<const T>;
	using MutByte =	typename std::conditional_t<std::is_const_v<T>, const Byte, Byte>;	
public:

	constexpr MutSpan() = default;
	constexpr MutSpan(T* data, Int size) : _data(data), _size(size) {}
	constexpr MutSpan(T& data) : _data(&data), _size(1) {}

	constexpr MutSpan	span		()			{ return MutSpan(_data, _size); }
	constexpr CSpan		span		() const	{ return   CSpan(_data, _size); }
	constexpr CSpan		constSpan	() const	{ return   CSpan(_data, _size); }

	constexpr operator CSpan() const { return constSpan(); }

	AX_INLINE constexpr bool inBound(Int      i) const { return i >= 0 && i < _size; }
	AX_INLINE constexpr bool inBound(IntRange r) const { return IntRange(0, _size).contains(r); }

	AX_INLINE constexpr bool isOverlapped(CSpan rhs) const { return MemoryUtil::isOverlapped(_data, _size, rhs.data(), rhs.size()); }

	AX_INLINE constexpr       T& operator[](Int i)       { return get(i); }
	AX_INLINE constexpr const T& operator[](Int i) const { return get(i); }

	AX_INLINE constexpr       T& get(Int i)       { _checkBound(i); return unsafe_get(i); }
	AX_INLINE constexpr const T& get(Int i) const { _checkBound(i); return unsafe_get(i); }

	AX_INLINE constexpr       T* try_get(Int i)       { return inBound(i) ? &unsafe_get(i) : nullptr; }
	AX_INLINE constexpr const T* try_get(Int i) const { return inBound(i) ? &unsafe_get(i) : nullptr; }
	
	AX_INLINE constexpr       T& back()       { return get(_size - 1); }
	AX_INLINE constexpr const T& back() const { return get(_size - 1); }

	AX_INLINE constexpr       T& back(Int i)       { return get(_size - i - 1); }
	AX_INLINE constexpr const T& back(Int i) const { return get(_size - i - 1); }

	AX_INLINE constexpr       T& unsafe_get(Int i)       { _debug_checkBound(i); return _data[i]; }
	AX_INLINE constexpr const T& unsafe_get(Int i) const { _debug_checkBound(i); return _data[i]; }

	AX_INLINE constexpr       T& unsafe_back(Int i)       { return unsafe_get(_size - i - 1); }
	AX_INLINE constexpr const T& unsafe_back(Int i) const { return unsafe_get(_size - i - 1); }

	AX_INLINE constexpr       T* data()       { return _data; }
	AX_INLINE constexpr const T* data() const { return _data; }
	AX_INLINE constexpr Int      size() const { return _size; }
	AX_INLINE constexpr Int	     sizeInBytes() const { return _size * ax_sizeof<T>; }

	AX_INLINE	constexpr static MutSpan	s_fromMutByteSpan	(MutSpan<MutByte>	from)	{ return MutSpan(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / ax_sizeof<T>); }
	AX_INLINE	constexpr static CSpan		s_fromByteSpan		(ByteSpan			from)	{ return   CSpan(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / ax_sizeof<T>); }

	AX_INLINE	constexpr void				fromMutByteSpan		(MutSpan<MutByte>	from)	{ *this = s_fromMutByteSpan(from); }
	AX_INLINE	constexpr void				fromByteSpan		(ByteSpan			from)	{ *this = s_fromByteSpan(from); }

	AX_INLINE	constexpr MutSpan<MutByte>	toMutByteSpan		() const { return MutSpan<MutByte>(reinterpret_cast<   MutByte*>(_data), sizeInBytes()); }
	AX_INLINE	constexpr    Span<Byte>		toByteSpan			() const { return MutSpan<MutByte>(reinterpret_cast<const Byte*>(_data), sizeInBytes()); }
	
	//            +--------------------------------------------+
	//            |         |                      |           |
	//            +--------------------------------------------+
	//  slice     ( offset )^------- size ---------^
	//  sliceFrom ( offset )^------------ to End --------------^
	//  sliceBack                                  ^---- N ----^
	//  sliceTill ^--------------------------------^(  N less  )

	AX_INLINE	constexpr MutSpan	slice		(Int offset, Int size);
	AX_INLINE	constexpr   CSpan	slice		(Int offset, Int size) const	{ return ax_const_cast(this)->slice(offset, size); }

	AX_INLINE	constexpr MutSpan	slice		(IntRange range)				{ return slice(range.start, range.size); }
	AX_INLINE	constexpr   CSpan	slice		(IntRange range) const			{ return slice(range.start, range.size); }

	AX_INLINE	constexpr MutSpan	sliceFrom	(Int offset)					{ return slice(offset, _size - offset); }
	AX_INLINE	constexpr   CSpan	sliceFrom	(Int offset) const				{ return slice(offset, _size - offset); }

	AX_INLINE	constexpr MutSpan	sliceBack	(Int n)							{ return slice(_size - n, n); }
	AX_INLINE	constexpr   CSpan	sliceBack	(Int n) const					{ return slice(_size - n, n); }	
	
	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()			{ return _data; }
	constexpr CIter	begin	() const	{ return _data; }
	constexpr  Iter	end		()			{ return _data + _size; }
	constexpr CIter	end		() const	{ return _data + _size; }
	
protected:
	AX_INLINE constexpr void _checkBound(Int i) const { if (!inBound(i)) throw Error_IndexOutOfRange(); }
	AX_INLINE constexpr void _debug_checkBound( Int i ) const {
#if _DEBUG
		_checkBound(i);
#endif
	}
	
	T*	_data = nullptr;
	Int _size = 0;
};

template <class T> AX_INLINE
constexpr MutSpan<T> MutSpan<T>::slice(Int offset, Int size) {
	if (offset < 0 || size < 0 || offset + size > _size) {
		throw Error_IndexOutOfRange();
	}
	return MutSpan(_data + offset, size);
}
} // namespace
