module;
#include "AxCore-pch.h"

export module AxCore.WPtr;

export import AxCore.SPtr;
import AxCore.SpinLock;
import AxCore.IArray;
import AxCore.Atomic;

export namespace ax {

template <bool USE_ATOMIC_PTR>
struct WPtrBlock_ : public SPtrReferenable_<USE_ATOMIC_PTR> {
	using LockType = std::conditional_t<USE_ATOMIC_PTR, Thread::SpinLock, Thread::NullSpinLock>;
	
	struct Data {
		SPtrReferenable* obj = nullptr;
	};
	Thread::LockProtected<LockType, Data> data;
};

template<bool USE_ATOMIC_PTR>
class WPtrReferenable_ : public SPtrReferenable_<USE_ATOMIC_PTR> {
public:
	using WPtrBlock = WPtrBlock_<USE_ATOMIC_PTR>;
	~WPtrReferenable_() {
		if (auto* block = _weakPtrBlock.ptr()) {
			AX_ASSERT(block->data.scopedLock()->obj == nullptr);
		}
	}
	SPtr<WPtrBlock>		_weakPtrBlock;
};

using WPtrReferenable = WPtrReferenable_<true>;

//! Weak pointer
template<class T>
class WPtr { // copyable
public:
	using WPtrBlock = typename T::WPtrBlock;
	
	WPtr() = default;
	WPtr(std::nullptr_t)	{}
	WPtr(T* p)				{ _ref(p); }

	template<class R>
	WPtr(WPtr<R> &r)	{ _ref(r.ptr()); }

	~WPtr() { unref(); }

	void ref(T* p) { _ref(p); }
	void ref(const SPtr<T>& r) { _ref(ax_const_cast(r.ptr())); }

	void unref	() { _block.unref(); }

	void operator=(std::nullptr_t)		{ unref(); }
	void operator=(const SPtr<T>& r)	{ _ref(ax_const_cast(r.ptr())); }

	static WPtr<T> s_ref(T* p) { return WPtr(p); }

	SPtr<T> getSPtr() {
		if (!_block) return nullptr;
		auto data = _block->data.scopedLock();
		return SPtr_ref(static_cast<T*>(data->obj));
	}

	SPtr<const T> getSPtr() const {
		if (!_block) return nullptr;
		auto data = ax_const_cast(_block->data).scopedLock();
		return SPtr_ref(static_cast<const T*>(data->obj));
	}

private:
	void _move(WPtr && r)	{ _block.move(r._block); }

	void _ref(T* p) {
		static_assert(std::is_base_of_v<WPtrReferenable, T>);

		if (!p) return _block.unref();
		
		if (auto* b = p->_weakPtrBlock.ptr()) {
			_block.ref(b);
			return;
		}

		_block.ref(new(AX_ALLOC_REQ) WPtrBlock());
		p->_weakPtrBlock.ref(_block.ptr());
		auto data = _block->data.scopedLock();
		data->obj = p;

		AX_ASSERT(static_cast<void*>(data->obj) == static_cast<void*>(p)); //T has multiple inheritance ?
	}

	SPtr<WPtrBlock>	_block;
};

template<class T> inline
void WPtrArray_append(IArray<WPtr<T>> & outArr, MutSpan<SPtr<T>> src) {
	outArr.reserve(outArr.size() + src.size());
	for (auto& s : src) {
		outArr.emplaceBack(s);
	}
}

template<class T> inline
void WPtrArray_assign(IArray<WPtr<T>> & outArr, MutSpan<SPtr<T>> src) {
	outArr.clear();
	WPtrArray_append(outArr, src);
}

} // namespace