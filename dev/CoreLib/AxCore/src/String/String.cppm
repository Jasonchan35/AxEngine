export module AxCore.String;

#include "AxBase.h"
export import AxCore.IString;
export import AxCore.Array;
import AxCore.Allocator;
import AxCore.MemoryUtil;
import AxCore.InlineBuffer;

export namespace ax {

template<class T> inline
constexpr Int String_DefaultBufSize = []() {
	Int size = 0;
	Int s = ax_sizeof<Int> / ax_sizeof<T>;
	Int padding = s > 0 ? s - 1 : 0;; // -1 for null terminator
	return size + padding;
}();

template<CharType T, Int BUF_SIZE = String_DefaultBufSize<T>>
class StringT_;
using String   = StringT_<Char  >;
using StringW  = StringT_<CharW >;
using String8  = StringT_<Char8 >;
using String16 = StringT_<Char16>;
using String32 = StringT_<Char32>;

template<Int N> using String_   = StringT_<Char  , N>;
template<Int N> using StringW_  = StringT_<CharW , N>;
template<Int N> using String8_  = StringT_<Char8 , N>;
template<Int N> using String16_ = StringT_<Char16, N>;
template<Int N> using String32_ = StringT_<Char32, N>;

template <CharType T>
using TempString_  = StringT_<T, 512>; // long enough to hold file path
using TempString   = TempString_<Char>;
using TempStringW  = TempString_<CharW>;
using TempString16 = TempString_<Char16>;
using TempString32 = TempString_<Char32>;


template<CharType T, Int BUF_SIZE> 
class StringT_ : public IString_<T>, InlineBuffer<T, BUF_SIZE + 1> // +1 for null terminator
{
	using Base = IString_<T>;
	using BaseInlineBuffer = InlineBuffer<T, BUF_SIZE + 1>;
	using BaseInlineBuffer::inlineBufPtr;
public:
	using View = StrView_<T>;
	
	AX_INLINE constexpr StringT_() : Base(inlineBufPtr(), BUF_SIZE) {}
	AX_INLINE constexpr StringT_(View view) : StringT_() { Base::append(view); }
	
	AX_INLINE constexpr StringT_(StringT_ && rhs) : StringT_() { Base::operator=(std::move(rhs.asIString())); }

	      IString_<T>& asIString()			{ return *this; }
	const IString_<T>& asIString() const	{ return *this; }

	virtual	~StringT_() override { Base::clearAndFree(); }

protected:
	virtual MemoryBlock<T>	onStorageLocalBuf() override { return MemoryBlock<T>(nullptr, inlineBufPtr(), BUF_SIZE); }
	virtual	MemoryBlock<T>	onStorageMalloc(Int reqSize) override;
	virtual	void			onStorageFree	(T* p) override;
};

template <CharType T, Int BUF_SIZE> inline
MemoryBlock<T> StringT_<T, BUF_SIZE>::onStorageMalloc(Int reqSize) {
	Int newCapacity = reqSize + 1; // +1 for null terminator
	auto* allocator = ax_default_allocator();
	auto buf = allocator->alloc<T>(newCapacity);
	buf.size--; // -1 for null terminator
	return buf;
}

template <CharType T, Int BUF_SIZE> inline
void StringT_<T, BUF_SIZE>::onStorageFree(T* p) {
	auto* d = inlineBufPtr(); 
	if (p == d) return;
	auto* allocator = ax_default_allocator();
	allocator->dealloc(p);
}



} // namespace
