module;


export module AxCore.Span;
export import AxCore.BasicMath;
export import AxCore.Range;
export import AxCore.MemUtil;

import AxCore.MemUtil;

export namespace ax {

template<class T> class MutSpan;
template<class T> using Span = MutSpan<const T>;

template<class T> constexpr bool Type_IsSpan = false;
template<class T> constexpr bool Type_IsSpan<Span<T>> = true;

using    ByteSpan	=    Span<Byte>;
using MutByteSpan	= MutSpan<Byte>;

using    IntSpan	=    Span<Int>;
using MutIntSpan	= MutSpan<Int>;

//! Reverse Enumerator
template<class T>
class	Span_RevForEach_ {
	using This = Span_RevForEach_;
public:
	constexpr operator Span_RevForEach_<const T>() const { return Span_RevForEach_<const T>(_begin, _end); }
	
	class	Iter {
	public:
		constexpr Iter( T* p=nullptr ) : _p(p) {}
		constexpr operator const T*	()  { return  _p; }
		constexpr T &		operator*	()	{ return *_p; }
		constexpr void	operator++	()	{ --_p; }
		constexpr bool	operator==	( const Iter & rhs )	{ return _p == rhs._p; }
		constexpr bool	operator!=	( const Iter & rhs )	{ return _p != rhs._p; }
	private:
		T*	_p;
	};

	constexpr Iter	begin	()	const	{ return Iter(_begin); }
	constexpr Iter	end		()	const	{ return Iter(_end);   }

	constexpr static This s_make(T* begin, T* end) {
		if (begin && end) {
			return This(end - 1, begin - 1);
		} else {
			return This(nullptr, nullptr);
		}
	}

private:
	Span_RevForEach_(T* begin, T* end) : _begin(begin), _end(end) {}
	
	T*		_begin;
	T*		_end;
};

template<class T>
class Span_InterleaveForEach_ {
	using This = Span_InterleaveForEach_;
public:
	operator Span_InterleaveForEach_<const T>() const { return Span_InterleaveForEach_<const T>(_begin, _end, _stride); }

	class Iter {
	public:
		constexpr Iter( T* p, Int stride ) : _p(p), _stride(stride) {}
		constexpr operator const T*	()			{ return  _p; }
		constexpr T&		operator*	()			{ return *_p; }
		constexpr T*		operator->	()			{ return  _p; }
		constexpr void	operator++	()			{ _p = MemUtil::addOffsetInBytes(_p, _stride); }
		constexpr void	operator++	(int)		{ _p = MemUtil::addOffsetInBytes(_p, _stride); }
		constexpr void	operator+=	(Int n)		{ _p = MemUtil::addOffsetInBytes(_p, _stride * n); }
		constexpr bool	operator==	(const Iter & rhs)	{ return _p == rhs._p; }
		constexpr bool	operator!=	(const Iter & rhs)	{ return _p != rhs._p; }
	protected:
		T*		_p;
		Int	_stride; // in bytes
	};

	constexpr AX_INLINE This slice(IntRange range) {
		auto* s = MemUtil::addOffsetInBytes(_begin, range.begin() * _stride);
		auto* e = MemUtil::addOffsetInBytes(_begin, range.end()   * _stride);
		if (s < _begin || e < _begin || s > _end || e > _end) {
			throw Error_IndexOutOfRange();
		}
		return This(s, e, _stride);		
	}

	constexpr AX_INLINE Iter	begin	()	const	{ return Iter(_begin, _stride); }
	constexpr AX_INLINE Iter	end		()	const	{ return Iter(_end,   _stride); }
	constexpr AX_INLINE Iter	operator[](Int i)	{ return at(i); }

	constexpr AX_INLINE	Int	size() const { return _stride ? MemUtil::sizeInBytes(_begin, _end) / _stride : 0; }

	constexpr AX_INLINE Iter	at		(Int i) {
		auto* p = MemUtil::addOffsetInBytes(_begin, i * _stride);
#ifdef AX_BUILD_CONFIG_Debug
		if (p < _begin || p + 1 > _end) {
			throw Error_IndexOutOfRange();
		}
#endif
		return Iter(p, _stride);		
	}

	constexpr void fillValues(const T& v) {
		for (auto& dst : *this) {
			dst = v;
		}
	}

	void fillRotateValues(Span<T> values) {
		if (values.size() == 0) return;

		Int i = 0;
		for (auto& dst : *this) {
			dst = values.at(i);
			i = (i + 1) % values.size();
		}
	}

	constexpr explicit Span_InterleaveForEach_(T* begin, T* end, Int stride) : _begin( begin ), _end( end ), _stride(stride) {}

private:
	T*		_begin;
	T*		_end;
	Int		_stride; // in bytes
};

template<class T>
struct Span_FindResult {
	Int	index;
	T&	value;

	using CResult = Span_FindResult<const T>;
	
	constexpr Span_FindResult(Int index_, T& value_) : index(index_), value(value_) {}
	
	constexpr CResult constResult() const { return CResult(index, value); }
	constexpr operator CResult() const { return constResult(); }
};

template<class T>
class MutSpan { // copyable
	using This = MutSpan;
	using MutByte =	typename std::conditional_t<std::is_const_v<T>, const Byte, Byte>;
	using MSpan = MutSpan<T>;
	using CSpan = MutSpan<const T>;
public:

	AX_INLINE constexpr MutSpan() = default;
	AX_INLINE constexpr MutSpan(const MutSpan& r) noexcept = default;
	AX_INLINE constexpr MutSpan(T* data, Int size) noexcept : _data(data), _size(size) {}
	// explicit ? 
	AX_INLINE constexpr MutSpan(T& t) noexcept : _data(&t), _size(1) {}
	
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

	template<class R, class Func>
	constexpr CmpResult compare(Span<R> r, Func func) const noexcept;

	template<class R>
	constexpr bool	    equals (Span<R> r) const noexcept;

	constexpr bool operator==(CSpan r) const noexcept { return  equals(r); }

	template<class R>
	constexpr bool operator==(Span<R> r) const noexcept { return equals(r); }

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
	AX_NODISCARD AX_INLINE	constexpr MSpan	slice			(IntRange range)				{ return slice(range.begin(), range.size()); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	slice			(IntRange range) const			{ return slice(range.begin(), range.size()); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	sliceBack		(Int newSize)			 		{ return slice(_size - newSize, newSize); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	sliceBack		(Int newSize) const	 			{ return slice(_size - newSize, newSize); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	sliceFrom		(Int offset)					{ return slice(offset, _size - offset); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	sliceFrom		(Int offset) const				{ return slice(offset, _size - offset); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	sliceFromBack	(Int offset)					{ return slice(0, _size - offset); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	sliceFromBack	(Int offset) const				{ return slice(0, _size - offset); }

	using MFindResult = Span_FindResult<T>;
	using CFindResult = Span_FindResult<const T>;

	template<class R>
	constexpr Opt<MFindResult> find(const R& r) {
		return find_([&r](const T& e){ return e == r; });
	}
	
	template<class FUNC>
	constexpr Opt<MFindResult> find_(FUNC func) {
		static_assert(Type_IsFuncSig<bool (const T&)>);
		auto* p = _data;
		auto* e = _data + _size;
		for (; p < e; ++p) {
			if (func(*p)) return MFindResult(p - _data, *p);
		}
		return std::nullopt;
	}

	template<class R>    constexpr Opt<CFindResult> find(const R& r) const { return ax_const_cast(this)->find(r); }
	template<class FUNC> constexpr Opt<CFindResult> find_(FUNC func) const { return ax_const_cast(this)->find_(func); }

	template<class FUNC> constexpr MFindResult findMin_(FUNC func) {
		static_assert(Type_IsFuncSig<bool (const T&, const T&)>);
		if (_size <= 0) throw Error_InvalidSize();

		auto* pMinValue = _data;
		auto* p = _data + 1;
		auto* e = _data + _size;
		for (; p < e; p++) {
			if (func(*p, *pMinValue)) {
				pMinValue = p;
			}
		}
		return MFindResult(pMinValue - _data, *p);
	}

	template<class FUNC> constexpr CFindResult findMin_(FUNC func) const { ax_const_cast(this)->findMin_(func); }
	constexpr MFindResult findMin()			{ return findMin_(ax_op_less<T>); }
	constexpr CFindResult findMin() const	{ return ax_const_cast(this)->findMin_(); }
	
	constexpr void sort() { sort(ax_op_less<T,T>); }
	
	template<class FUNC> constexpr void sort(FUNC func) {
		static_assert(Type_IsFuncSig<bool (const T&, const T&)>);
		// simple sorting, prevent move data if possible
		for (Int i = 0; i < _size; i++) {
			Int minIndex = i + sliceFrom(i).findMin_(func).index;
			if (minIndex == i) continue;

			std::swap(at(minIndex), at(i));
			minIndex = i;
		}
	}

	constexpr void copyValues(CSpan v, Int offset = 0);
	constexpr void fillValues(const T& v);

	constexpr Opt<Int> getIndexFromElementPtr(const T* element) const;

	template<class TT> using Iter = TT*; 
	constexpr Iter<T>		begin	()		 noexcept	{ return _data; }
	constexpr Iter<const T>	begin	() const noexcept	{ return _data; }
	constexpr Iter<T>		end		()		 noexcept	{ return _data + _size; }
	constexpr Iter<const T>	end		() const noexcept	{ return _data + _size; }

	template<class TT> using RevForEach_ = Span_RevForEach_<TT>;
	constexpr auto revForEach	()			{ return RevForEach_<      T>::s_make( _data, _data + _size ); }
	constexpr auto revForEach	() const	{ return RevForEach_<const T>::s_make( _data, _data + _size ); }
	
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

template<class OBJ, class T>
class Span_BaseFunc {
	using MSpan = MutSpan<T>;
	using CSpan =    Span<T>;
	constexpr MSpan _obj_span()			{ return static_cast<      OBJ*>(this)->span(); }
	constexpr CSpan _obj_span() const	{ return static_cast<const OBJ*>(this)->span(); }
public:
	AX_NODISCARD AX_INLINE constexpr bool isOverlapped(CSpan rhs) const noexcept { return _obj_span().isOverlapped(rhs); }

	template<class R, class Func>
	constexpr CmpResult compare(Span<R> r, Func func) const noexcept { return _obj_span().compare(r, func); }

	template<class R>
	constexpr bool	    equals (Span<R> r) const noexcept { return _obj_span().equals(r); }

	constexpr bool operator==(CSpan r) const noexcept { return _obj_span() == r; }

	template<class R>
	constexpr bool operator==(Span<R> r) const noexcept { return _obj_span() == r; }


	
	AX_NODISCARD AX_INLINE constexpr ByteSpan	toByteSpan()		{ return _obj_span().toByteSpan(); }
	AX_NODISCARD AX_INLINE constexpr ByteSpan	toByteSpan() const	{ return _obj_span().toByteSpan(); }

	//                +--------------------------------------------+
	//                |         |                      |           |
	//                +--------------------------------------------+
	//  slice         (  offset  )[______ newSize ____]
	//  sliceBack                       [________ newSize__________]
	//  sliceFrom     (  offset  )[_________till to end ___________]
	//  sliceFromBack [________________________________](  offset  )
	AX_NODISCARD AX_INLINE	constexpr MSpan	slice			(Int offset, Int newSize)		{ return _obj_span().slice(offset, newSize); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	slice			(Int offset, Int newSize) const	{ return _obj_span().slice(offset, newSize); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	slice			(IntRange range)				{ return _obj_span().slice(range); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	slice			(IntRange range) const			{ return _obj_span().slice(range); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	sliceBack		(Int newSize)			 		{ return _obj_span().slice(newSize); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	sliceBack		(Int newSize) const	 			{ return _obj_span().slice(newSize); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	sliceFrom		(Int offset)					{ return _obj_span().slice(offset); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	sliceFrom		(Int offset) const				{ return _obj_span().slice(offset); }
	AX_NODISCARD AX_INLINE	constexpr MSpan	sliceFromBack	(Int offset)					{ return _obj_span().slice(offset); }
	AX_NODISCARD AX_INLINE	constexpr CSpan	sliceFromBack	(Int offset) const				{ return _obj_span().slice(offset); }

	using MFindResult = Span_FindResult<T>;
	using CFindResult = Span_FindResult<const T>;

	template<class TARGET, class FUNC>
	constexpr Opt<CFindResult> find(const TARGET& target, FUNC func) const {
		return _obj_span().template find<TARGET, FUNC>(target, func);
	}

	template<class TARGET>
	constexpr Opt<CFindResult> find(const TARGET& target) const {
		return _obj_span().template find<TARGET>(target);
	}
	
	template<class FUNC>
	constexpr CFindResult findMin(FUNC func) const	{ return _obj_span().template findMin<FUNC>(func); }
	constexpr CFindResult findMin() const			{ return _obj_span().findMin(); }
	
	template<class FUNC>
	constexpr void	sort(FUNC func)	{ _obj_span().template sort<FUNC>(func); }
	constexpr void	sort()			{ _obj_span().sort(); }


	constexpr void copyValues(CSpan v, Int offset = 0)	{ _obj_span().copyValues(v, offset); }
	constexpr void fillValues(const T& v)				{ _obj_span().fillValues(v); }
	
	constexpr Opt<Int> getIndexFromElementPtr(const T* element) const { return _obj_span().getIndexFromElementPtr(element); }

	template<class TT> using Iter = TT*; 
	constexpr Iter<T>		begin	()		 noexcept	{ return _obj_span().begin(); }
	constexpr Iter<const T>	begin	() const noexcept	{ return _obj_span().begin(); }
	constexpr Iter<T>		end		()		 noexcept	{ return _obj_span().end(); }
	constexpr Iter<const T>	end		() const noexcept	{ return _obj_span().end(); }

	template<class TT> using RevForEach_ = Span_RevForEach_<TT>;
	constexpr auto revForEach	()			{ return _obj_span().revForEach(); }
	constexpr auto revForEach	() const	{ return _obj_span().revForEach(); }
};

template<class T> constexpr 
void MutSpan<T>::copyValues(CSpan v, Int offset) {
	if (offset < 0 || offset + v.size() > _size) throw Error_IndexOutOfRange();
	MemUtil::copy(_data + offset, v.data(), v.size());
}

template<class T> constexpr
void MutSpan<T>::fillValues(const T& v) {
	T* p = _data;
	T* end = _data + _size;
	for (; p < end; p++) {
		*p = v;
	}
}

template<class T> constexpr
Opt<Int> MutSpan<T>::getIndexFromElementPtr(const T* element) const {
	const T* s = _data;
	const T* e = _data + _size;
	if (element < s || element >= e) return std::nullopt;
	return element - _data;
}

template<class T>
template<class R>
constexpr bool MutSpan<T>::equals(Span<R> r) const noexcept {
	if (_size == 0 && r.size() == 0) return true;
	if (_size != r.size()) return false;
	if constexpr (Type_IsSame<R,T>) {
		if (_data != r.data()) return true;
	}

	if (!std::is_constant_evaluated()) { AX_ASSERT(_size >= 0); }
	if (_size <= 0) return true;
	
	auto* s = _data;
	auto* e = _data + _size;
	auto* d = r.data();
	for (; s<e; ++s, ++d) {
		if (*s != *d) return false;
	}
	return true;

}

template<class T>
template<class R, class Func>
constexpr CmpResult MutSpan<T>::compare(Span<R> r, Func func) const noexcept {
	if (_size == 0 && r.size() == 0) return CmpResult::Equal;
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
