module;


export module AxCore.UPtr;

export import AxCore.BasicType;
export import AxCore.Allocator;
export import AxCore.Span;

export namespace ax {

//! Unique Pointer
template<class T, class DEL = AxDelete>
class UPtr : public PtrBase<T>, public NonCopyable {
	using Base = PtrBase<T>;
	using Base::_p;
	
	void operator=(const UPtr&) = delete;
public:
	AX_INLINE	UPtr() = default;
	AX_INLINE	UPtr(UPtr && r) noexcept { move(AX_FORWARD(r)); }
	AX_INLINE	UPtr(std::nullptr_t) noexcept {}
	
	 // only accept when using same DEL class
	template<class R> AX_INLINE	UPtr(const UPtr<R, DEL> & r) = delete;

	 // only accept when using same DEL class
	template<class R> AX_INLINE	UPtr(UPtr<R, DEL> && r) noexcept { move(AX_FORWARD(r)); }

	template<class... ARGS>
	AX_INLINE	UPtr(AxTag::NewObject_, const MemAllocRequest& req, ARGS&&... args) { newObject(req, AX_FORWARD(args)...); }

	AX_INLINE	~UPtr() noexcept { unref(); }

	operator       T* () &			{ return _p; }
	operator const T* () const &	{ return _p; }

//	operator T* () && = delete;

	AX_INLINE	T*		ref			(T* p) & noexcept	{ if (_p != p) { unref(); _p = p; } return p; }
	AX_INLINE	void	unref		() noexcept			{ DEL::deleteObject(_p); _p = nullptr; }
	AX_INLINE	T*		detach		() noexcept			{ auto* p = _p; _p = nullptr; return p; }

	template<class... ARGS>
	T*	newObject(const MemAllocRequest& req, ARGS&&...args) {
		return ref(new (req) T(AX_FORWARD(args)...));
	}

	template<class R>
	AX_INLINE	void move(UPtr<R, DEL> && r) noexcept;

	AX_INLINE	bool operator==(const T* p) const noexcept { return _p == p; }
	AX_INLINE	bool operator!=(const T* p) const noexcept { return _p != p; }

	AX_INLINE	void operator=(UPtr && r) noexcept { move(AX_FORWARD(r)); }

	template<class R>
	AX_INLINE	void operator=(UPtr<R, DEL> && r) noexcept { move(AX_FORWARD(r)); }

	static UPtr  s_ref(T* p) noexcept { return UPtr(p); }
	static UPtr  s_ref_DontAddRefCount(T* p) noexcept { return UPtr(p); }

private:
	//! not allowed in public, please use UPtr_ref or UPtr<T>::s_ref
	AX_INLINE	explicit UPtr(T* p) : Base(p) {}

	// not allowed for ownership safety reason
	// operator T* () { return _p; }
};

template<class T> constexpr bool Type_IsUPtr = false; 
template<class T> constexpr bool Type_IsUPtr<UPtr<T>> = true; 

template<class T, class DEL> AX_INLINE bool operator==(const T* a,				const UPtr<T, DEL>& b) { return b == a; }
template<class T, class DEL> AX_INLINE bool operator==(const std::nullptr_t&,	const UPtr<T, DEL>& b) { return b == nullptr; }

template<class T, class DEL = AxDelete> AX_INLINE UPtr<T, DEL> UPtr_ref(T* p) noexcept { return UPtr<T, DEL>::s_ref(p); }

template<class T, class DEL>
template<class R> AX_INLINE
void UPtr<T, DEL>::move(UPtr<R, DEL> && r) noexcept {
//	if (static_cast<void*>(&r) == this) return;
	if (r.ptr() == _p) return;
	static_assert(std::is_base_of_v<T, R>);
	ref(r.detach());
}

template<class T, class DEL = AxDelete, class... ARGS> AX_INLINE
UPtr<T, DEL> UPtr_new(const MemAllocRequest& req, ARGS &&... args) {
	return UPtr_ref<T, DEL>(new (req) T(AX_FORWARD(args)...));
}

template<class DST, class SRC> AX_INLINE
UPtr<DST> UPtr_static_cast(UPtr<SRC> && src) {
	return UPtr<DST>::s_ref(static_cast<DST*>(src.detach()));
}

template<class T, class DEL = AxDelete> AX_INLINE
MutSpan<T * const> UPtrSpan_to_PtrSpan(MutSpan<UPtr<T, DEL>> src) {
	static_assert(sizeof(T*) == sizeof(UPtr<T, DEL>));
	return MutSpan<T * const>(reinterpret_cast<T* const *>(src.data()), src.size());
}


} // namespace