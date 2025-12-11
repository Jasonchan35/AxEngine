module;


export module AxCore.IArray;
export import AxCore.IArrayStorage;

export namespace ax {

template<class T>
class IArray : public IArrayStorage<T>, public Span_BaseFunc<IArray<T>, T> {
	using This = IArray;
	using Base = IArrayStorage<T>;
	using Base::_storage;
protected:
	constexpr IArray(T* data, Int initCap) : Base(data, initCap) {}
public:
	using MSpan      = MutSpan<T>;
	using CSpan      = Span<T>;

	AX_NODISCARD AX_INLINE constexpr       T*	data()				{ return _storage.data(); }
	AX_NODISCARD AX_INLINE constexpr const T*	data() const		{ return _storage.data(); }
	AX_NODISCARD AX_INLINE constexpr 	Int		capacity() const	{ return _storage.capacity(); }
	AX_NODISCARD AX_INLINE constexpr	Int		size() const		{ return _storage.size(); }
	AX_NODISCARD AX_INLINE constexpr 	Int		sizeInBytes() const noexcept { return size() * AX_SIZEOF(T); }

	AX_NODISCARD AX_INLINE	bool inBound( Int  i ) const	{ return i >= 0 && i < size(); }

	constexpr void clear() { Base::_storageClear(); }
	constexpr void clearAndFree() { Base::_storageClearAndFree(); }
	
//	constexpr void reserve(Int newCapacity) { ensureCapacity(newCapacity); }
	constexpr void ensureCapacity(Int newCapacity) { Base::_storageEnsureCapacity(newCapacity); }
	
	template<class... Args> constexpr void resize(Int newSize, Args&&... args) { Base::_storageResize(newSize, AX_FORWARD(args)...); }
	template<class... Args> constexpr void incSize(Int n,   Args&&... args) { resize(size() + n, AX_FORWARD(args)...); }
							constexpr void decSize(Int n) { resize(size() - n); }
							constexpr void resizeToCapacity() { resize(capacity()); }
							constexpr void ensureSize(Int n) { if (size() < n) resize(n); }

	template< class... Args >
	AX_INLINE	T& emplaceBack(Args&&... args)	{ resize(size() + 1, AX_FORWARD(args)...); return back(); }

	template< class... Args >
	AX_INLINE	T&	emplaceNew(const MemAllocRequest& req, Args&&... args ) { return emplaceBack(AxTag::NewObject, req, AX_FORWARD(args)...); }
	
	constexpr void append(const T& item);
	constexpr void append(T && item);
	constexpr void appendRange(Span<T> src);

	template<class R, class FUNC>
	constexpr void appendRange(Span<R> src, FUNC func);
	
	AX_INLINE constexpr void operator<<(const T &  item)  { append(item); }
	AX_INLINE constexpr void operator<<(      T && item)  { append(AX_FORWARD(item)); }

	AX_INLINE constexpr void operator=(      CSpan   src) { Base::_storageCopy(src); }
	AX_INLINE constexpr void operator=(const This &  src) { Base::_storageCopy(src); }
	AX_INLINE constexpr void operator=(      This && src) { Base::_storageMove(AX_FORWARD(src)); }

	AX_NODISCARD AX_INLINE constexpr MSpan	span		()			{ return MutSpan<T>(data(), size()); }
	AX_NODISCARD AX_INLINE constexpr CSpan	span		() const	{ return    Span<T>(data(), size()); }
	AX_NODISCARD AX_INLINE constexpr CSpan	constSpan	() const	{ return    Span<T>(data(), size()); }

	AX_NODISCARD AX_INLINE constexpr operator MSpan()				{ return span(); }
	AX_NODISCARD AX_INLINE constexpr operator CSpan() const			{ return span(); }

	AX_NODISCARD AX_INLINE constexpr       T& operator[](Int i)       noexcept { return at(i); }
	AX_NODISCARD AX_INLINE constexpr const T& operator[](Int i) const noexcept { return at(i); }

	AX_NODISCARD AX_INLINE constexpr       T& at(Int i)       noexcept					{ _checkBound(i); return at_noBoundCheck(i); }
	AX_NODISCARD AX_INLINE constexpr const T& at(Int i) const noexcept					{ _checkBound(i); return at_noBoundCheck(i); }
	AX_NODISCARD AX_INLINE constexpr       T* tryGetElement(Int i)       noexcept		{ return inBound(i) ? &at_noBoundCheck(i) : nullptr; }
	AX_NODISCARD AX_INLINE constexpr const T* tryGetElement(Int i) const noexcept		{ return inBound(i) ? &at_noBoundCheck(i) : nullptr; }

	//TODO: remove
	AX_NODISCARD AX_INLINE constexpr       T* tryGet(Int i)       noexcept				{ return tryGetElement(i); }
	AX_NODISCARD AX_INLINE constexpr const T* tryGet(Int i) const noexcept				{ return tryGetElement(i); }
	
	AX_NODISCARD AX_INLINE constexpr       T& back()       noexcept 					{ return at(size() - 1); }
	AX_NODISCARD AX_INLINE constexpr const T& back() const noexcept 					{ return at(size() - 1); }
	AX_NODISCARD AX_INLINE constexpr       T& back(Int i)       noexcept				{ return at(size() - i - 1); }
	AX_NODISCARD AX_INLINE constexpr const T& back(Int i) const noexcept				{ return at(size() - i - 1); }
	AX_NODISCARD AX_INLINE constexpr       T& at_noBoundCheck(Int i)       noexcept		{ _debug_boundCheck(i); return data()[i]; }
	AX_NODISCARD AX_INLINE constexpr const T& at_noBoundCheck(Int i) const noexcept		{ _debug_boundCheck(i); return data()[i]; }
	AX_NODISCARD AX_INLINE constexpr       T& back_noBoundCheck(Int i)       noexcept	{ return at_noBoundCheck(size() - i - 1); }
	AX_NODISCARD AX_INLINE constexpr const T& back_noBoundCheck(Int i) const noexcept	{ return at_noBoundCheck(size() - i - 1); }

	AX_NODISCARD AX_INLINE constexpr T& ensureSizeAndGet(Int i)		{ ensureSize(i+1); return at(i); }
	
	AX_INLINE constexpr		T&	insertAt	(Int i)				{ return *insertAt(IntRange(i, 1)).data(); }
	AX_INLINE constexpr		T&	insertAt	(Int i, T && value)	{ auto dst = insertAt(IntRange(i, 1)); *dst.data() = std::move(value); return *dst.data(); }
	AX_INLINE constexpr	MSpan	insertAt	(IntRange range);

	AX_INLINE constexpr		T	popBack()					{ T tmp = back(); decSize(1); return tmp; }
	
	AX_INLINE constexpr	Int		eraseAt	(Int i)				{ return eraseAt(IntRange(i, 1)); }
	AX_INLINE constexpr	Int		eraseAt	(IntRange range);
	AX_INLINE constexpr	void	eraseAt_Unordered( Int i );

	template<class FUNC> constexpr Int	eraseIf			(FUNC func);
	template<class FUNC> constexpr Int	eraseIf_Unordered(FUNC func);
	template<class FUNC> constexpr Int	eraseAllIf		(FUNC func);

	template<class V> AX_INLINE	constexpr Int	eraseIfEquals( const V& v )				{ return eraseIf([&](auto& e) { return e == v; }); }
	template<class V> AX_INLINE	constexpr Int	eraseIfEquals_Unordered( const V& v )	{ return eraseIf_Unordered([&](auto& e) { return e == v; }); }
	template<class V> AX_INLINE constexpr Int	eraseAllIfEquals(const V& v)			{ return eraseAllIf([&](auto& e) { return e == v; }); }
	

	constexpr bool operator==(CSpan r) const noexcept { return span() == r; }

	template<class R>
	constexpr bool operator==(Span<R> r) const noexcept { return span() == r; }
	constexpr bool operator==(const This& r) const noexcept { return span() == r.span(); }

	template<class SE> void onJsonIO_Value(SE& se) { se.io_array(*this); }
	
	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()			{ return data(); }
	constexpr CIter	begin	() const	{ return data(); }
	constexpr  Iter	end		()			{ return data() + size(); }
	constexpr CIter	end		() const	{ return data() + size(); }

private:

	AX_INLINE void _checkBound			( Int i ) const { if( ! inBound(i) ) throw Error_IndexOutOfRange(); }
	AX_INLINE void	_debug_boundCheck	( Int i ) const {
#ifdef AX_BUILD_CONFIG_Debug
		_checkBound(i);
#endif
	}	
};

template <class T>
template <class FUNC> constexpr
Int IArray<T>::eraseIf(FUNC func) {
	Int oldSize = size();
	auto* p = data();
	for (Int i = 0; i < oldSize; i++) {
		if (func(p[i])) {
			eraseAt(i);
			return 1;
		}
	}
	return 0;
}

template <class T>
template <class FUNC> constexpr
Int IArray<T>::eraseIf_Unordered(FUNC func) {
	Int oldSize = size();
	auto* p = data();
	for (Int i = 0; i < oldSize; i++) {
		if (func(p[i])) {
			eraseAt_Unordered(i);
			return 1;
		}
	}
	return 0;
}

template <class T>
template <class FUNC> constexpr 
Int IArray<T>::eraseAllIf(FUNC func) {
	Int	erasedCount = 0;

	auto* p = data();
	auto oldSize = size();
	
	auto* src	 = p;
	auto* end	 = p + oldSize;
	auto* target = p;

	while(src < end) {
		if (func(*src)) {
			erasedCount++;
			src++;
			continue;
		}
		
		if (target != src) {
			*target = std::move(*src);
		}

		src++;
		target++;
	}

	resize(oldSize - erasedCount);
	return erasedCount;	
}

template<class T> constexpr bool Type_IsIArray = false; 
template<class T> constexpr bool Type_IsIArray<IArray<T>> = true; 

template <typename OBJ>
concept CON_IsIArray = requires (OBJ obj) {
	[]<typename T>(const IArray<T>&){}(obj); 
};

template <class T>
constexpr void IArray<T>::append(const T& item) {
	auto oldSize = size();
	auto newSize = oldSize + 1;
	ensureCapacity(newSize);
	ax_call_constructor<T>(data() + oldSize, item);
	_storage.setSize(newSize);
}

template <class T>
constexpr void IArray<T>::append(T && item) {
	auto oldSize = size();
	auto newSize = oldSize + 1;
	ensureCapacity(newSize);
	ax_call_constructor<T>(data() + oldSize, AX_FORWARD(item));
	_storage.setSize(newSize);
}

template <class T>
constexpr void IArray<T>::appendRange(Span<T> src) {
	auto oldSize = size();
	auto newSize = oldSize + src.size();
	ensureCapacity(newSize);
	MemUtil::copyConstructor(data() + oldSize, src.data(), src.size());
	_storage.setSize(newSize);
}

template <class T>
template <class R, class FUNC>
constexpr void IArray<T>::appendRange(Span<R> src, FUNC func) {
//	static_assert(std::is_convertible_v<FUNC, const R&>);
	auto oldSize = size();
	auto newSize = oldSize + src.size();
	ensureCapacity(newSize);
	MemUtil::copyConstructorFunc(data() + oldSize, src.data(), src.size(), func);
	_storage.setSize(newSize);
}

template <class T>
constexpr typename IArray<T>::MSpan IArray<T>::insertAt(IntRange range) {
	if (range.size() <= 0) return;
	auto oldSize = size();
	if (range.begin() > oldSize) { AX_ASSERT(false); return; }

	auto newSize = oldSize + range.size();
	resize(newSize);

	auto* p = data();

	auto* src = p + range.begin() + range.size() - 1;
	auto* end = p + oldSize - range.size();
	auto* dst = end + range.size();
	auto* tail = p + size();
	if (end < p || end > tail
	 || dst < p || dst > tail)
	{
		AX_ASSERT(false);
		throw Error_IndexOutOfRange();
	}

	MemUtil::moveConstructor(dst, src, range.size());
	return MutSpan<T>(p, range.size());
}

template <class T>
constexpr Int IArray<T>::eraseAt(IntRange range) {
	if (range.begin() < 0 || range.size() < 0) {
		AX_ASSERT(false);
		return 0;
	}

	auto* p = data();
	Int oldSize = size();
	
	auto* dst = p + range.begin();
	auto* end = p + oldSize;

	if (dst >= end) { AX_ASSERT(false); return 0; }

	Int erasedCount = 0;

	auto* src = p + range.end();
	if (src >= end) {
		AX_ASSERT(src == end);
		erasedCount = end - dst;

	} else {
		AX_ASSERT(dst < src);

		for (;src < end; ++src, ++dst) {
			*dst = std::move(*src);
		}
		erasedCount = src - dst;
	}

	AX_ASSERT(erasedCount == range.size());
	resize(oldSize - erasedCount);
	return erasedCount;	
}

template <class T>
constexpr void IArray<T>::eraseAt_Unordered(Int i) {
	_checkBound(i);
	auto oldSize = size();
	if (oldSize < 1) { clear(); return; }
	auto* p = data();
	p[i] = std::move(p[oldSize - 1]);
	decSize(1);
}

} // namespace
