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
	using BaseInlineBuffer = InlineBuffer<T, BUF_SIZE>;
	using Base = IArray<T>;

	using BaseInlineBuffer::inlineBufPtr;
public:
	Array() : Base(inlineBufPtr(), BUF_SIZE) {}
	virtual	~Array() override { Base::clearAndFree(); }
	
protected:
	virtual	MemoryBlock	onMalloc(Int reqSize) override;
	virtual	void		onFree	(T* p) override;
};

template <class T, Int BUF_SIZE> inline
MemoryBlock Array<T, BUF_SIZE>::onMalloc(Int reqSize) {
	printf("onMalloc %lld\n", reqSize);
	
	auto* allocator = ax_default_allocator();
	return allocator->alloc(reqSize * AX_SIZE_OF(T), AX_ALIGN_OF(T));
}

template <class T, Int BUF_SIZE> inline
void Array<T, BUF_SIZE>::onFree(T* p) {
}
} // namespace

