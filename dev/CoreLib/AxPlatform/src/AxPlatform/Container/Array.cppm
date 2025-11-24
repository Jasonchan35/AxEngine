module;
#include "AxPlatform-pch.h"

export module AxPlatform.Array;

export import AxPlatform.IArray;
import AxPlatform.Allocator;
import AxPlatform.MemoryUtil;
import AxPlatform.InlineBuffer;

export namespace ax {

template <class T, Int BUF_SIZE = 0> class Array;
using ByteArray = Array<Byte>;
using IntArray  = Array<Int>;

template <class T, Int BUF_SIZE>
class Array : public IArray<T>, InlineBuffer<T, BUF_SIZE> {
	using Base = IArray<T>;
	using BaseInlineBuffer = InlineBuffer<T, BUF_SIZE>;
	using BaseInlineBuffer::inlineBufPtr;
public:
	Array() : Base(inlineBufPtr(), BUF_SIZE) {}
	virtual	~Array() override { Base::clearAndFree(); }
	
protected:
	virtual MemAllocResult<T>	onStorageLocalBuf() override { return MemAllocResult<T>(nullptr, inlineBufPtr(), BUF_SIZE); }
	virtual	MemAllocResult<T>	onStorageMalloc(Int reqSize) override;
	virtual	void			onStorageFree(T* p) override;
};

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

