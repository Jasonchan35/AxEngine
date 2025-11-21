export module AxCore.String;

#include "AxBase.h"
export import AxCore.IString;
export import AxCore.Array;
import AxCore.Allocator;
import AxCore.MemoryUtil;
import AxCore.InlineBuffer;

export namespace ax {

template<CharType T, Int BUF_SIZE = 0> class String_;
using String   = String_<Char  >;
using StringW  = String_<CharW >;
using String8  = String_<Char8 >;
using String16 = String_<Char16>;
using String32 = String_<Char32>;

template<Int N> using String_N   = String_<Char  , N>;
template<Int N> using StringW_N  = String_<CharW , N>;
template<Int N> using String8_N  = String_<Char8 , N>;
template<Int N> using String16_N = String_<Char16, N>;
template<Int N> using String32_N = String_<Char32, N>;

template<class T> using TempString_ = String_<T, 512>; // long enough to hold file path
using TempString	= TempString_< Char   >;
using TempStringW	= TempString_< CharW  >;
using TempString16	= TempString_< Char16 >;
using TempString32	= TempString_< Char32 >;


template<CharType T, Int BUF_SIZE> 
class String_ : public IString_<T>, InlineBuffer<T, BUF_SIZE + 1> // +1 for null  terminator
{
	using Base = IString_<T>;
	using BaseInlineBuffer = InlineBuffer<T, BUF_SIZE + 1>;
	using BaseInlineBuffer::inlineBufPtr;
public:
	using View = StrView_<T>;
	
	AX_INLINE constexpr String_() : Base(inlineBufPtr(), BUF_SIZE) {}
	AX_INLINE constexpr String_(View view) : String_() { Base::append(view); }
	
	AX_INLINE constexpr String_(String_ && rhs) : String_() { Base::operator=(std::move(rhs.asIString())); }

	      IString_<T>& asIString()			{ return *this; }
	const IString_<T>& asIString() const	{ return *this; }

	virtual	~String_() override { Base::clearAndFree(); }

protected:
	virtual MemoryBlock<T>	onStorageLocalBuf() override { return MemoryBlock<T>(nullptr, inlineBufPtr(), BUF_SIZE); }
	virtual	MemoryBlock<T>	onStorageMalloc(Int reqSize) override;
	virtual	void			onStorageFree	(T* p) override;
};

template <CharType T, Int BUF_SIZE> inline
MemoryBlock<T> String_<T, BUF_SIZE>::onStorageMalloc(Int reqSize) {
	if (reqSize <= BUF_SIZE) {
		return MemoryBlock<T>(nullptr, inlineBufPtr(), BUF_SIZE);
	}
	
	Int newCapacity = reqSize;

	constexpr Int kMinByteSize = 64;
	newCapacity = Math::max(newCapacity, kMinByteSize / ax_sizeof<T>);

	if (newCapacity < 2048) {
		newCapacity = Math::nextPow2_half(newCapacity);
	}
	
	auto* allocator = ax_default_allocator();
	return allocator->alloc<T>(newCapacity);
}

template <CharType T, Int BUF_SIZE> inline
void String_<T, BUF_SIZE>::onStorageFree(T* p) {
	auto* d = inlineBufPtr(); 
	if (p == d) return;
	auto* allocator = ax_default_allocator();
	allocator->dealloc(p);
}



} // namespace
