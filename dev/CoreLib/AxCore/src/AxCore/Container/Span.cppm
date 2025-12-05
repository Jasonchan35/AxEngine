module;
#include "AxCore-pch.h"

export module AxCore.Span;
export import AxCore.BasicMath;
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

	AX_INLINE constexpr MutSpan() = default;
	AX_INLINE constexpr MutSpan(T* data, Int size) noexcept : _data(data), _size(size) {}

	template<Int N>
	AX_INLINE constexpr MutSpan(T (&p)[N]) noexcept : _data(p), _size(N) {}
	
	AX_INLINE constexpr void setPtr(T* data, Int size) noexcept { _data = data; _size = size; }
	AX_NODISCARD constexpr bool samePtrAndSize(CSpan r) const noexcept { return _size == r._size && _data == r._data; }
	
	AX_NODISCARD AX_INLINE constexpr MSpan	span		()       noexcept	{ return MSpan(_data, _size); }
	AX_NODISCARD AX_INLINE constexpr CSpan	span		() const noexcept	{ return CSpan(_data, _size); }
	AX_NODISCARD AX_INLINE constexpr CSpan	constSpan	() const noexcept	{ return CSpan(_data, _size); }
	AX_NODISCARD AX_INLINE constexpr operator CSpan() const noexcept { return constSpan(); }

	AX_NODISCARD AX_INLINE constexpr bool inBound(Int      i) const noexcept { return i >= 0 && i < _size; }
	AX_NODISCARD AX_INLINE constexpr bool inBound(IntRange r) const noexcept { return IntRange(0, _size).contains(r); }

	AX_NODISCARD AX_INLINE constexpr bool isOverlapped(CSpan rhs) const noexcept { return MemUtil::isOverlapped(_data, _size, rhs.data(), rhs.size()); }

	AX_NODISCARD AX_INLINE constexpr       T& operator[](Int i)       noexcept { return at(i); }
	AX_NODISCARD AX_INLINE constexpr const T& operator[](Int i) const noexcept { return at(i); }

	AX_NODISCARD AX_INLINE constexpr       T& at(Int i)       noexcept			{ _checkBound(i); return unsafe_at(i); }
	AX_NODISCARD AX_INLINE constexpr const T& at(Int i) const noexcept			{ _checkBound(i); return unsafe_at(i); }
	AX_NODISCARD AX_INLINE constexpr       T* try_at(Int i)       noexcept		{ return inBound(i) ? &unsafe_at(i) : nullptr; }
	AX_NODISCARD AX_INLINE constexpr const T* try_at(Int i) const noexcept		{ return inBound(i) ? &unsafe_at(i) : nullptr; }
	AX_NODISCARD AX_INLINE constexpr       T& back()       noexcept 			{ return at(_size - 1); }
	AX_NODISCARD AX_INLINE constexpr const T& back() const noexcept 			{ return at(_size - 1); }
	AX_NODISCARD AX_INLINE constexpr       T& back(Int i)       noexcept		{ return at(_size - i - 1); }
	AX_NODISCARD AX_INLINE constexpr const T& back(Int i) const noexcept		{ return at(_size - i - 1); }
	AX_NODISCARD AX_INLINE constexpr       T& unsafe_at(Int i)       noexcept	{ _debug_checkBound(i); return _data[i]; }
	AX_NODISCARD AX_INLINE constexpr const T& unsafe_at(Int i) const noexcept	{ _debug_checkBound(i); return _data[i]; }
	AX_NODISCARD AX_INLINE constexpr       T& unsafe_back(Int i)       noexcept	{ return unsafe_at(_size - i - 1); }
	AX_NODISCARD AX_INLINE constexpr const T& unsafe_back(Int i) const noexcept	{ return unsafe_at(_size - i - 1); }

	AX_NODISCARD AX_INLINE constexpr       T* data()       noexcept { return _data; }
	AX_NODISCARD AX_INLINE constexpr const T* data() const noexcept { return _data; }
	AX_NODISCARD AX_INLINE constexpr Int      size() const noexcept { return _size; }
	AX_NODISCARD AX_INLINE constexpr Int      sizeInBytes() const noexcept { return _size * AX_SIZEOF(T); }

	template<class Func>
	constexpr CmpResult compare(CSpan r, Func func) const noexcept;
	constexpr bool	    equals (CSpan r) const noexcept;

	constexpr bool operator==(CSpan r) const noexcept { return  equals(r); }
	constexpr bool operator!=(CSpan r) const noexcept { return !equals(r); }

	AX_NODISCARD AX_INLINE	constexpr static MSpan		s_fromMutByteSpan	(MutByteSpan	from) noexcept	{ return MSpan(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / AX_SIZEOF(T)); }
	AX_NODISCARD AX_INLINE	constexpr static CSpan		s_fromByteSpan		(   ByteSpan	from) noexcept	{ return CSpan(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / AX_SIZEOF(T)); }

				 AX_INLINE	constexpr void				fromMutByteSpan		(MutByteSpan	from) noexcept	{ *this = s_fromMutByteSpan(from); }
				 AX_INLINE	constexpr void				fromByteSpan		(ByteSpan		from) noexcept	{ *this = s_fromByteSpan(from); }

	AX_NODISCARD AX_INLINE	constexpr MutByteSpan		toMutByteSpan		()       noexcept { return MutByteSpan(reinterpret_cast<   MutByte*>(_data), sizeInBytes()); }
	AX_NODISCARD AX_INLINE	constexpr    ByteSpan		toByteSpan			() const noexcept { return    ByteSpan(reinterpret_cast<const Byte*>(_data), sizeInBytes()); }
	
	//                +--------------------------------------------+
	//                |         |                      |           |
	//                +--------------------------------------------+
	//  slice         (  offset  )[______ newSize ____]
	//  sliceBack                       [________ newSize__________]
	//  sliceFrom     (  offset  )[_________till to end ___________]
	//  sliceFromBack [________________________________](  offset  )
	AX_NODISCARD AX_INLINE	constexpr MSpan	slice			(Int offset, Int newSize) {
		if (offset < 0 || newSize < 0 || offset + newSize > _size) throw Error_IndexOutOfRange();
		return MSpan(_data + offset, newSize);
	}
	AX_NODISCARD AX_INLINE	constexpr CSpan	slice			(Int offset, Int newSize) const	{ return ax_const_cast(this)->slice(offset, newSize); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	slice			(IntRange range)				{ return slice(range.start, range.size); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	slice			(IntRange range) const			{ return slice(range.start, range.size); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	sliceBack		(Int newSize)			 		{ return slice(_size - newSize, newSize); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	sliceBack		(Int newSize) const	 			{ return slice(_size - newSize, newSize); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	sliceFrom		(Int offset)					{ return slice(offset, _size - offset); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	sliceFrom		(Int offset) const				{ return slice(offset, _size - offset); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	sliceFromBack	(Int offset)					{ return slice(0, _size - offset); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	sliceFromBack	(Int offset) const				{ return slice(0, _size - offset); }

	struct FindResult {
		FindResult(Int index_, const T& value_) : index(index_), value(value_) {}
		Int			index;
		const T&	value;
	};
	template<class FuncOp = FuncOp_Less<T>> constexpr FindResult	findMin	() const;
	template<class FuncOp = FuncOp_Less<T>> constexpr void			sort	();

	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()		 noexcept	{ return _data; }
	constexpr CIter	begin	() const noexcept	{ return _data; }
	constexpr  Iter	end		()		 noexcept	{ return _data + _size; }
	constexpr CIter	end		() const noexcept	{ return _data + _size; }
	
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

template <class T>
template<class FuncOp> constexpr
auto MutSpan<T>::findMin() const -> FindResult {
	if (_size <= 0) throw Error_InvalidSize();

	auto* pMinValue = _data;
	auto* p = _data + 1;
	auto* e = _data + _size;
	for (; p < e; p++) {
		if (FuncOp::invoke(*p, *pMinValue)) {
			pMinValue = p;
		}
	}
	return FindResult(pMinValue - _data, *p);
}

template <class T>
template <class FuncOp> constexpr 
void MutSpan<T>::sort() {
	// simple sorting, prevent move data if possible
	for (Int i = 0; i < _size; i++) {
		Int minIndex = i + sliceFrom(i).findMin<FuncOp>().index;
		if (minIndex == i) continue;

		std::swap(at(minIndex), at(i));
		minIndex = i;
	}
}

template <class T>
constexpr bool MutSpan<T>::equals(CSpan r) const noexcept {
	if (_size == 0 && r._size == 0) return true;
	if (_size != r.size()) return false;
	if (_data != r.data()) return true;
	return MemUtil::equals(_data, r.data(), _size);
}

template<class T>
template<class Func>
constexpr CmpResult MutSpan<T>::compare(CSpan r, Func func) const noexcept {
	if (_size == 0 && r._size == 0) return CmpResult::Equal;
	if (samePtrAndSize(r)) return CmpResult::Equal;

	Int n = Math::min(_size, r.size());
	const T* p0 = _data;
	const T* p1 = r.data();

	Int i = 0;
	for( i=0; i<n; ++p0, ++p1, i++ ) {
		auto res = func(*p0, *p1);
		if (res != CmpResult::Equal) return res;
	}

	return CmpResult_fromInt(_size - r.size());
}

} // namespace
