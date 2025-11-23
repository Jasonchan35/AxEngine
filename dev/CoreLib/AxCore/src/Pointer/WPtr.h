#pragma once

#include "SPtr.h"
#include "AxCore/Thread/SpinLock.h"
#include "AxCore/Container/IArray.h"

namespace ax {

struct WPtrBlock : public SPtrReferenable {
	struct Data {
		SPtrReferenable* obj = nullptr;
	};
	SpinLockProtected<Data>	data;
};

class WPtrReferenable : public SPtrReferenable {
	using Base = SPtrReferenable;
public:

	virtual ~WPtrReferenable() override {
		if (_weakBlock) {
			auto data = _weakBlock->data.scopedLock();
			AX_ASSERT(data->obj == nullptr); // _clearWPtrBlock() should be called onRefCountZero() to avoid race condition during destructor
			data->obj = nullptr;
		}
	}

	virtual void onRefCountZero() override {
		clearWPtrBlock();
		Base::onRefCountZero();
	}

	void clearWPtrBlock() {
		if (_weakBlock) {
			auto data = _weakBlock->data.scopedLock();
			data->obj = nullptr;
		}
	}
	SPtr<WPtrBlock>	_weakBlock;
};

//! Weak pointer
template<class T>
class WPtr {
public:
	WPtr() = default;
	WPtr(std::nullptr_t)	{}
	WPtr(T* p)				{ _ref(p); }
	WPtr(const WPtr&  r)	{ _block = r._block; }
	WPtr(      WPtr&& r)	{ _block = std::move(r._block); }

	template<class R>
	WPtr(WPtr<R> &r)	{ _ref(r.ptr()); }

	~WPtr() {
		unref();
	}

	void ref(T* p) { _ref(p); }
	void ref(const SPtr<T>& r) { _ref(ax_const_cast(r.ptr())); }

	void unref	() { _block.unref(); }

	void operator=(const WPtr & r)		{ _ref(r._p); }
	void operator=(WPtr && r)			{ _move(std::move(r)); }
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
		
		if (auto* b = p->_weakBlock.ptr()) {
			_block.ref(b);
			return;
		}

		_block.ref(new(AX_ALLOC_REQ) WPtrBlock());
		p->_weakBlock.ref(_block.ptr());
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