module;


export module AxCore.WPtr;

export import AxCore.SPtr;
import AxCore.SpinLock;
import AxCore.Atomic;

export namespace ax {

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
		static_assert(std::is_base_of_v<WPtr_Referenceable, T>);

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

enum class SmartPtrType {
	None,
	UPtr,
	SPtr,
	WPtr,
};

template<class T, SmartPtrType E> struct SmartPtr_Struct;
template<class T> struct SmartPtr_Struct<T, SmartPtrType::UPtr> { using Type = UPtr<T>; };
template<class T> struct SmartPtr_Struct<T, SmartPtrType::SPtr> { using Type = SPtr<T>; };
template<class T> struct SmartPtr_Struct<T, SmartPtrType::WPtr> { using Type = WPtr<T>; };

template<class T, SmartPtrType E> using SmartPtr_ = typename SmartPtr_Struct<T, E>::Type; 

} // namespace