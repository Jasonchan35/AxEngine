export module AxCore.InlineBuffer;

#include "AxBase.h"

import AxCore.BasicType;

export namespace ax {

template< class T, Int BUF_SIZE >
class InlineBuffer : public NonCopyable {
protected:
	static constexpr	Int	kInlineBufSize = BUF_SIZE;

	AX_INLINE			T*		inlineBufPtr	() 		 { return reinterpret_cast<	     T*>(_inlineBuf); }
	AX_INLINE	const 	T*		inlineBufPtr	() const { return reinterpret_cast<const T*>(_inlineBuf); }
	
	Byte	_inlineBuf[AX_SIZE_OF(T) * BUF_SIZE];
};

template< class T >
class InlineBuffer<T,0> : public NonCopyable {
protected:
	static constexpr	Int	kInlineBufSize = 0;
	AX_INLINE 			T*		inlineBufPtr	() 		 { return nullptr; }
	AX_INLINE 	const	T*		inlineBufPtr	() const { return nullptr; }
};

} // namespace

