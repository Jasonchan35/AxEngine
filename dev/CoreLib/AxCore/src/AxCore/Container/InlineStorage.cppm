module;
#include "AxCore-pch.h"

export module AxCore.InlineStorage;
import AxCore.BasicType;

export namespace ax {

template< class T, Int BUF_SIZE >
class InlineStorage : public NonCopyable {
protected:
	static constexpr	Int	kInlineBufSize = BUF_SIZE;
	
	AX_INLINE constexpr	T* inlineBufPtr() {
		AX_PRAGMA_GCC(diagnostic push)
		AX_PRAGMA_GCC(diagnostic ignored "-Wuninitialized")
		return reinterpret_cast<T*>(_inlineBuf);
		AX_PRAGMA_GCC(diagnostic pop)
	}
	
	AX_INLINE constexpr	const 	T*	inlineBufPtr() const { return ax_const_cast(this)->inlineBufPtr(); }
	
private:
//	std::aligned_storage<sizeof(T) * BUF_SIZE, alignof(T)> _inlineBuf;
	alignas(T) Byte _inlineBuf[AX_SIZEOF(T) * BUF_SIZE];
};

template< class T >
class InlineStorage<T,0> : public NonCopyable {
protected:
	static constexpr	Int	 kInlineBufSize = 0;
	AX_INLINE constexpr	      T* inlineBufPtr	() 		 { return nullptr; }
	AX_INLINE constexpr	const T* inlineBufPtr	() const { return nullptr; }
};

} // namespace

