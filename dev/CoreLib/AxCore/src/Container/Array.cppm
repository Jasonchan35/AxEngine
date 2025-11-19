export module AxCore.Array;

export import AxCore.IArray;
import AxCore.Allocator;
import AxCore.MemoryUtil;
import AxCore.InlineBuffer;

#include "AxBase.h"

export namespace ax {

template <class T, Int BUF_SIZE = 0>
class Array;

template <class T, Int BUF_SIZE>
class Array : public IArray<T>, InlineBuffer<T, BUF_SIZE> {
	using Base = IArray<T>;
	using BaseInlineBuffer = InlineBuffer<T, BUF_SIZE>;
	using BaseInlineBuffer::inlineBufPtr;
public:
	Array() : Base(inlineBufPtr(), BUF_SIZE) {}
	virtual	~Array() override { Base::clearAndFree(); }
	
protected:
	virtual	MemoryBlock<T>	onMalloc(Int reqSize) override;
	virtual	void			onFree	(T* p) override;
};

template <class T, Int BUF_SIZE> inline
MemoryBlock<T> Array<T, BUF_SIZE>::onMalloc(Int reqSize) {
	if (reqSize <= BUF_SIZE) {
		return MemoryBlock<T>(nullptr, inlineBufPtr(), BUF_SIZE);
	}
	
	Int newCapacity = reqSize;
	if (reqSize < 2048) {
		newCapacity = Math::nextPow2_half(reqSize);
	}
	
	auto* allocator = ax_default_allocator();
	return allocator->alloc<T>(newCapacity);
}

template <class T, Int BUF_SIZE> inline
void Array<T, BUF_SIZE>::onFree(T* p) {
	if (p == inlineBufPtr()) return;
	auto* allocator = ax_default_allocator();
	allocator->dealloc(p);	
}
} // namespace

