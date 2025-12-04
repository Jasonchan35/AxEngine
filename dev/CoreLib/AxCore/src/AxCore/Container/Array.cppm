module;
#include "AxCore-pch.h"

export module AxCore.Array;

export import AxCore.IArray;
import AxCore.Allocator;
import AxCore.MemoryUtil;
import AxCore.InlineStorage;

export namespace ax {

template <class T, Int BUF_SIZE = 0> class Array;
using IByteArray = IArray<Byte>;
using  ByteArray =  Array<Byte>;
using IIntArray  = IArray<Int>;
using  IntArray  =  Array<Int>;

template <class T, Int BUF_SIZE>
class Array : public IArray<T>, InlineStorage<T, BUF_SIZE> {
	using This = Array;
	using Base = IArray<T>;
	using BaseInlineBuffer = InlineStorage<T, BUF_SIZE>;
	using BaseInlineBuffer::inlineBufPtr;
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

} // namespace

