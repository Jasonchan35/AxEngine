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
	using MutByte =	typename std::conditional_t<std::is_const_v<T>, const Byte, Byte>;
	using MSpan = MutSpan<T>;
	using CSpan = MutSpan<const T>;
public:

	constexpr MutSpan() = default;
	constexpr MutSpan(T* data, Int size) noexcept : _data(data), _size(size) {}
	constexpr MutSpan(T& data) noexcept : _data(&data), _size(1) {}

	constexpr MutSpan	span		()       noexcept	{ return MutSpan(_data, _size); }
	constexpr CSpan		span		() const noexcept	{ return   CSpan(_data, _size); }
	constexpr CSpan		constSpan	() const noexcept	{ return   CSpan(_data, _size); }

	constexpr operator CSpan() const noexcept { return constSpan(); }

	AX_INLINE constexpr bool inBound(Int      i) const noexcept { return i >= 0 && i < _size; }
	AX_INLINE constexpr bool inBound(IntRange r) const noexcept { return IntRange(0, _size).contains(r); }

	AX_INLINE constexpr bool isOverlapped(CSpan rhs) const noexcept { return MemUtil::isOverlapped(_data, _size, rhs.data(), rhs.size()); }

	AX_INLINE constexpr       T& operator[](Int i)       noexcept { return at(i); }
	AX_INLINE constexpr const T& operator[](Int i) const noexcept { return at(i); }

	AX_INLINE constexpr       T& at(Int i)       noexcept { _checkBound(i); return unsafe_at(i); }
	AX_INLINE constexpr const T& at(Int i) const noexcept { _checkBound(i); return unsafe_at(i); }

	AX_INLINE constexpr       T* try_at(Int i)       noexcept { return inBound(i) ? &unsafe_at(i) : nullptr; }
	AX_INLINE constexpr const T* try_at(Int i) const noexcept { return inBound(i) ? &unsafe_at(i) : nullptr; }
	
	AX_INLINE constexpr       T& back()       noexcept { return at(_size - 1); }
	AX_INLINE constexpr const T& back() const noexcept { return at(_size - 1); }

	AX_INLINE constexpr       T& back(Int i)       noexcept { return at(_size - i - 1); }
	AX_INLINE constexpr const T& back(Int i) const noexcept { return at(_size - i - 1); }

	AX_INLINE constexpr       T& unsafe_at(Int i)       noexcept { _debug_checkBound(i); return _data[i]; }
	AX_INLINE constexpr const T& unsafe_at(Int i) const noexcept { _debug_checkBound(i); return _data[i]; }

	AX_INLINE constexpr       T& unsafe_back(Int i)       noexcept { return unsafe_at(_size - i - 1); }
	AX_INLINE constexpr const T& unsafe_back(Int i) const noexcept { return unsafe_at(_size - i - 1); }

	AX_INLINE constexpr       T* data()       noexcept { return _data; }
	AX_INLINE constexpr const T* data() const noexcept { return _data; }
	AX_INLINE constexpr Int      size() const noexcept { return _size; }
	AX_INLINE constexpr Int	     sizeInBytes() const noexcept { return _size * ax_sizeof<T>; }

	AX_INLINE	constexpr static MSpan		s_fromMutByteSpan	(MutByteSpan	from) noexcept	{ return MSpan(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / ax_sizeof<T>); }
	AX_INLINE	constexpr static CSpan		s_fromByteSpan		(   ByteSpan	from) noexcept	{ return CSpan(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / ax_sizeof<T>); }

	AX_INLINE	constexpr void				fromMutByteSpan		(MutByteSpan	from) noexcept	{ *this = s_fromMutByteSpan(from); }
	AX_INLINE	constexpr void				fromByteSpan		(ByteSpan		from) noexcept	{ *this = s_fromByteSpan(from); }

	AX_INLINE	constexpr MutByteSpan		toMutByteSpan		()       noexcept { return MutByteSpan(reinterpret_cast<   MutByte*>(_data), sizeInBytes()); }
	AX_INLINE	constexpr    ByteSpan		toByteSpan			() const noexcept { return    ByteSpan(reinterpret_cast<const Byte*>(_data), sizeInBytes()); }
	
	//            +--------------------------------------------+
	//            |         |                      |           |
	//            +--------------------------------------------+
	//  slice     ( offset )^------- size ---------^
	//  sliceFrom ( offset )^------------ to End --------------^
	//  sliceBack                                  ^---- N ----^
	//  sliceTill ^--------------------------------^(  N less  )

	AX_INLINE	constexpr MSpan	slice		(Int offset, Int size);
	AX_INLINE	constexpr CSpan	slice		(Int offset, Int size) const	{ return ax_const_cast(this)->slice(offset, size); }

	AX_INLINE	constexpr MSpan	slice		(IntRange range)				{ return slice(range.start, range.size); }
	AX_INLINE	constexpr CSpan	slice		(IntRange range) const			{ return slice(range.start, range.size); }

	AX_INLINE	constexpr MSpan	sliceFrom	(Int offset)					{ return slice(offset, _size - offset); }
	AX_INLINE	constexpr CSpan	sliceFrom	(Int offset) const				{ return slice(offset, _size - offset); }

	AX_INLINE	constexpr MSpan	sliceBack	(Int n)			 				{ return slice(_size - n, n); }
	AX_INLINE	constexpr CSpan	sliceBack	(Int n) const	 				{ return slice(_size - n, n); }	
	
	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()		 noexcept	{ return _data; }
	constexpr CIter	begin	() const noexcept	{ return _data; }
	constexpr  Iter	end		()		 noexcept	{ return _data + _size; }
	constexpr CIter	end		() const noexcept	{ return _data + _size; }
	
protected:
	AX_INLINE constexpr void _checkBound(Int i) const noexcept { if (!inBound(i)) throw Error_IndexOutOfRange(); }
	AX_INLINE constexpr void _debug_checkBound( Int i ) const noexcept {
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
