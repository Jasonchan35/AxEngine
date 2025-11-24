module;
#include "AxPlatform-pch.h"

export module AxPlatform.InlineBuffer;
import AxPlatform.BasicType;

export namespace ax {

template< class T, Int BUF_SIZE >
class InlineBuffer : public NonCopyable {
protected:
	static constexpr	Int	kInlineBufSize = BUF_SIZE;
	
	AX_INLINE constexpr			T*		inlineBufPtr	() {
		AX_PRAGMA_GCC(diagnostic push)
		AX_PRAGMA_GCC(diagnostic ignored "-Wuninitialized")
		return reinterpret_cast<	     T*>(_inlineBuf);
		AX_PRAGMA_GCC(diagnostic pop)		
	}
	
	AX_INLINE constexpr	const 	T*		inlineBufPtr	() const { return ax_const_cast(this)->inlineBufPtr(); }
	
private:
	Byte	_inlineBuf[ax_sizeof<T> * BUF_SIZE];
};

template< class T >
class InlineBuffer<T,0> : public NonCopyable {
protected:
	static constexpr	Int	kInlineBufSize = 0;
	AX_INLINE constexpr	T*		inlineBufPtr	() 		 { return nullptr; }
	AX_INLINE constexpr	const	T*		inlineBufPtr	() const { return nullptr; }
};

} // namespace

