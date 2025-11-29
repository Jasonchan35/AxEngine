module;
#include "AxPlatform-pch.h"

export module AxPlatform.Array;

export import AxPlatform.IArray;
import AxPlatform.Allocator;
import AxPlatform.MemoryUtil;
import AxPlatform.InlineStorage;

export namespace ax {

template <class T, Int BUF_SIZE = 0> class Array;
using ByteArray = Array<Byte>;
using IntArray  = Array<Int>;

template <class T, Int BUF_SIZE>
class Array : public IArray<T>, InlineStorage<T, BUF_SIZE> {
	using Base = IArray<T>;
	using BaseInlineBuffer = InlineStorage<T, BUF_SIZE>;
	using BaseInlineBuffer::inlineBufPtr;
public:
	Array() : Base(inlineBufPtr(), BUF_SIZE) {}
	virtual	~Array() override { Base::clearAndFree(); }
	
protected:
	virtual MemAllocResult<T>	onStorageLocalBuf() override { return MemAllocResult<T>(nullptr, inlineBufPtr(), BUF_SIZE); }
	virtual	MemAllocResult<T>	onStorageMalloc(Int reqSize) override;
	virtual	void				onStorageFree(T* p) override;
};

template<class T>		 struct Type_IsArray_Struct : std::false_type {};
template<class T, Int N> struct Type_IsArray_Struct< Array<T,N> > : std::true_type {};
template<class T> constexpr bool Type_IsFixedArray = Type_IsArray_Struct<T>::value; 


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

