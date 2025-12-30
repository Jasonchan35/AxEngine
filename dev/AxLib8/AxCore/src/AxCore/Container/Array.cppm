module;
export module AxCore.Array;
export import AxCore.IArray;
export import AxCore.FixedArray;

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
	using CSpan = Span<T>;
	
	constexpr Array() : Base(inlineBufPtr(), BUF_SIZE) {}
	constexpr Array(      CSpan        src) : Array() { operator=(src);  }
	constexpr Array(const Array      & src) : Array(src.span()) {}
	constexpr Array(const IArray<T>  & src) : Array(src.span()) {}
	template<Int M>
	constexpr Array(const Array<T,M> & src) : Array(src.span()) {}

	constexpr Array(IArray<T> && src) : Array() { operator=(AX_FORWARD(src)); }
	constexpr Array(Array     && src) : Array(AX_FORWARD(src.asIArray())) {}

	constexpr virtual	~Array() override { Base::clearAndFree(); }

	constexpr 	  IArray<T>& asIArray()			{ return *this; }
	constexpr const IArray<T>& asIArray() const	{ return *this; }

	constexpr void operator=(      CSpan        src) { Base::operator=(src); } 
	constexpr void operator=(const IArray<T>  & src) { Base::operator=(src); }
	constexpr void operator=(const Array      & src) { Base::operator=(src.asIArray()); } 
	template<Int M>
	constexpr void operator=(const Array<T,M> & src) { Base::operator=(src.asIArray()); } 

	constexpr void appendRange(Span<T> src) { Base::appendRange(src); }

	template<class R, class FUNC>
	constexpr void appendRange(Span<R> src, FUNC func = [](const T& v){ return v; } ) { Base::appendRange(src, func); }
	
	template<Int M>
	constexpr void appendRange(Array<T,M> && src) { Base::appendRange(std::move(src.asIArray())); }

protected:
	constexpr virtual MemAllocResult<T>	onStorageLocalBuf() override { return MemAllocResult<T>(nullptr, inlineBufPtr(), BUF_SIZE); }
	constexpr virtual MemAllocResult<T>	onStorageMalloc(Int reqSize) override;
	constexpr virtual void				onStorageFree(T* p) override;
};

template<class T> constexpr bool Type_IsArray = false;
template<class T, Int N> constexpr bool Type_IsArray<Array<T,N>> = true;


template <class T, Int BUF_SIZE> constexpr 
MemAllocResult<T> Array<T, BUF_SIZE>::onStorageMalloc(Int reqSize) {
	Int newCapacity = reqSize;
	auto* allocator = ax_default_allocator();
	return allocator->allocArray<T>(newCapacity);
}

template <class T, Int BUF_SIZE> constexpr
void Array<T, BUF_SIZE>::onStorageFree(T* p) {
	if (p == inlineBufPtr()) return;
	auto* allocator = ax_default_allocator();
	allocator->dealloc(p);	
}



} // namespace

