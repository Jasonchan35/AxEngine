module;
#include "AxCore-pch.h"

export module AxCore.Array;

export import AxCore.IArray;
import AxCore.Allocator;
import AxCore.MemoryUtil;

export namespace ax {

template <class T, Int BUF_SIZE = 0> class Array;
using IByteArray = IArray<Byte>;
using  ByteArray =  Array<Byte>;
using IIntArray  = IArray<Int>;
using  IntArray  =  Array<Int>;

template< class T, Int BUF_SIZE >
class Array_InlineBuffer : public NonCopyable {
protected:
	static constexpr	Int	kInlineBufSize = BUF_SIZE;
	AX_INLINE constexpr	T* inlineBufPtr() { return reinterpret_cast<T*>(_inlineBuf); }
	AX_INLINE constexpr	const 	T*	inlineBufPtr() const { return ax_const_cast(this)->inlineBufPtr(); }
private:
	alignas(T) Byte _inlineBuf[AX_SIZEOF(T) * BUF_SIZE];
};

template< class T >
class Array_InlineBuffer<T,0> : public NonCopyable {
protected:
	static constexpr	Int	 kInlineBufSize = 0;
	AX_INLINE constexpr	      T* inlineBufPtr	() 		 { return nullptr; }
	AX_INLINE constexpr	const T* inlineBufPtr	() const { return nullptr; }
};

template <class T, Int BUF_SIZE>
class Array : public IArray<T>, Array_InlineBuffer<T, BUF_SIZE> {
	using This = Array;
	using Base = IArray<T>;
	using InlineBuffer = Array_InlineBuffer<T, BUF_SIZE>;
	using InlineBuffer::inlineBufPtr;
public:
	Array() : Base(inlineBufPtr(), BUF_SIZE) {}
	virtual	~Array() override { Base::clearAndFree(); }

		  IArray<T>& asIArray()			{ return *this; }
	const IArray<T>& asIArray() const	{ return *this; }
	
	constexpr void operator=(const IArray<T>& src) { asIArray() = src; } 
	
	constexpr void operator=(const This& src) { asIArray() = src; } 
	
protected:
	virtual MemAllocResult<T>	onStorageLocalBuf() override { return MemAllocResult<T>(nullptr, inlineBufPtr(), BUF_SIZE); }
	virtual	MemAllocResult<T>	onStorageMalloc(Int reqSize) override;
	virtual	void				onStorageFree(T* p) override;
};

template<class T> constexpr bool Type_IsArray = false;
template<class T, Int N> constexpr bool Type_IsArray<Array<T,N>> = true;


template <class T, Int BUF_SIZE> inline
MemAllocResult<T> Array<T, BUF_SIZE>::onStorageMalloc(Int reqSize) {
	Int newCapacity = reqSize;
	auto* allocator = ax_default_allocator();
	return allocator->allocArray<T>(newCapacity);
}

template <class T, Int BUF_SIZE> inline
void Array<T, BUF_SIZE>::onStorageFree(T* p) {
	if (p == inlineBufPtr()) return;
	auto* allocator = ax_default_allocator();
	allocator->dealloc(p);	
}


template<class T, Int N>
class FixedArray {
	T _data[N];
	
	using This = FixedArray;
public:
	using Element = T;
	static const Int kElementCount = N;

	FixedArray() = default;

	MutSpan<T>	  span		()			{ return MutSpan<T>(data(), size()); }
	   Span<T>	  span		() const	{ return    Span<T>(data(), size()); }
	   Span<T>	  constSpan	() const	{ return    Span<T>(data(), size()); }

	operator  MutSpan<T>	()			{ return span(); }
	operator  Span<T>		() const	{ return span(); }

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr    FixedSpan<T, N> fixedSpan() const { return    FixedSpan<T, N>(data()); }
	AX_INLINE constexpr MutFixedSpan<T, N> fixedSpan()       { return MutFixedSpan<T, N>(data()); }

	AX_INLINE constexpr	 	  T &	operator[]	( Int i )			{ return at(i); }
	AX_INLINE constexpr	const T &	operator[]	( Int i ) const		{ return at(i); }

	AX_INLINE constexpr	 	  T &	at			( Int i )			{ return fixedSpan().at(i); }
	AX_INLINE constexpr	const T &	at			( Int i ) const		{ return fixedSpan().at(i); }

	AX_INLINE constexpr	 		T *	try_at		(Int i)				{ return fixedSpan().try_at(i); }
	AX_INLINE constexpr	const	T *	try_at		(Int i) const		{ return fixedSpan().try_at(i); }

	//-----------------------
	AX_INLINE constexpr		Int		size		() const			{ return N; }
	AX_INLINE constexpr		bool	inBound		( Int  i ) const	{ return i >= 0 && i < N; }

	AX_INLINE constexpr 	  T*	data		()					{ return _data; }
	AX_INLINE constexpr	const T*	data		() const			{ return _data; }

	//------- forward View functions ----------------
	AX_INLINE constexpr		MutSpan<T>	slice		( Int offset, Int size )		{ return span().slice(offset, size); }
	AX_INLINE constexpr		Span<T>		slice		( Int offset, Int size ) const	{ return span().slice(offset, size); }

	AX_INLINE constexpr		MutSpan<T>	sliceFrom	( Int offset )					{ return span().sliceFrom(offset); }
	AX_INLINE constexpr		Span<T>		sliceFrom	( Int offset ) const			{ return span().sliceFrom(offset); }

	AX_INLINE constexpr		MutSpan<T>	sliceBack	( Int offset )					{ return span().sliceBack(offset); }
	AX_INLINE constexpr		Span<T>		sliceBack	( Int offset ) const			{ return span().sliceBack(offset); }

	AX_INLINE constexpr		void		copyValues	(Span<T> v, Int offset = 0)		{ span().copyValues(); }
	AX_INLINE constexpr		void		fillValues	( const T& v )					{ span().fillValues(v); }

	//---------------
	using  Iter			= T*;
	using CIter			= const T*;

	constexpr  Iter	begin	()			{ return data(); }
	constexpr CIter	begin	() const	{ return data(); }
	constexpr  Iter	end		()			{ return data() + size(); }
	constexpr CIter	end		() const	{ return data() + size(); }

private:
	AX_INLINE void _checkBound			( Int i ) const { if( ! inBound(i) ) throw Error_IndexOutOfRange(); }
	AX_INLINE void	_debug_checkBound	( Int i ) const {
		#ifdef AX_BUILD_CONFIG_Debug
			_checkBound(i);
		#endif
	}
};

template<class T> constexpr bool Type_IsFixedArray = false; 
template<class T, Int N> constexpr bool Type_IsFixedArray<FixedArray<T, N>> = true; 


} // namespace

