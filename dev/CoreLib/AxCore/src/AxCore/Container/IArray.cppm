module;
#include "AxCore-pch.h"

export module AxCore.IArray;
export import AxCore.IArrayStorage;

export namespace ax {

template<class T>
class IArray : public IArrayStorage<T> {
	using Base = IArrayStorage<T>;
	using Base::_storage;
protected:
	constexpr IArray(T* data, Int initCap) : Base(data, initCap) {}
	using MSpan = MutSpan<T>;
	using CSpan =    Span<T>;
public:
	AX_NODISCARD AX_INLINE constexpr T*  data() { return _storage.data(); }
	AX_NODISCARD AX_INLINE constexpr Int size() const { return _storage.size(); }
	AX_NODISCARD AX_INLINE constexpr Int capacity() const { return _storage.capacity(); }
	AX_NODISCARD AX_INLINE constexpr Int sizeInBytes() const noexcept { return size() * AX_SIZEOF(T); }

	AX_NODISCARD AX_INLINE	bool inBound( Int  i ) const	{ return i >= 0 && i < size(); }
	
	constexpr void clear() { Base::_storageClear(); }
	constexpr void clearAndFree() { Base::_storageClearAndFree(); }
	
	constexpr void reserve(Int newCapacity) { Base::_storageReserve(newCapacity); }
	
	template<class... Args>
	constexpr void resize(Int newSize, Args&&... args) { Base::_storageResize(newSize, AX_FORWARD(args)...); }

	template< class... Args >
	AX_INLINE	T& emplaceBack(Args&&... args)	{ resize(size() + 1, AX_FORWARD(args)...); return back(); }
	
	constexpr void append(const T& item);
	constexpr void append(T && item);

	constexpr void operator << (const T &  item)  { append(item); }
	constexpr void operator << (      T && item)  { append(AX_FORWARD(item)); }

	MSpan	span		()			{ return MutSpan<T>(data(), size()); }
	CSpan	span		() const	{ return    Span<T>(data(), size()); }
	CSpan	constSpan	() const	{ return    Span<T>(data(), size()); }

	operator  MutSpan<T>	()			{ return span(); }
	operator  Span<T>		() const	{ return span(); }

	AX_NODISCARD AX_INLINE constexpr       T& operator[](Int i)       noexcept { return at(i); }
	AX_NODISCARD AX_INLINE constexpr const T& operator[](Int i) const noexcept { return at(i); }

	AX_NODISCARD AX_INLINE constexpr       T& at(Int i)       noexcept			{ _checkBound(i); return unsafe_at(i); }
	AX_NODISCARD AX_INLINE constexpr const T& at(Int i) const noexcept			{ _checkBound(i); return unsafe_at(i); }
	AX_NODISCARD AX_INLINE constexpr       T* try_at(Int i)       noexcept		{ return inBound(i) ? &unsafe_at(i) : nullptr; }
	AX_NODISCARD AX_INLINE constexpr const T* try_at(Int i) const noexcept		{ return inBound(i) ? &unsafe_at(i) : nullptr; }
	AX_NODISCARD AX_INLINE constexpr       T& back()       noexcept 			{ return at(size() - 1); }
	AX_NODISCARD AX_INLINE constexpr const T& back() const noexcept 			{ return at(size() - 1); }
	AX_NODISCARD AX_INLINE constexpr       T& back(Int i)       noexcept		{ return at(size() - i - 1); }
	AX_NODISCARD AX_INLINE constexpr const T& back(Int i) const noexcept		{ return at(size() - i - 1); }
	AX_NODISCARD AX_INLINE constexpr       T& unsafe_at(Int i)       noexcept	{ _debug_checkBound(i); return data()[i]; }
	AX_NODISCARD AX_INLINE constexpr const T& unsafe_at(Int i) const noexcept	{ _debug_checkBound(i); return data()[i]; }
	AX_NODISCARD AX_INLINE constexpr       T& unsafe_back(Int i)       noexcept	{ return unsafe_at(size() - i - 1); }
	AX_NODISCARD AX_INLINE constexpr const T& unsafe_back(Int i) const noexcept	{ return unsafe_at(size() - i - 1); }
	
	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()			{ return data(); }
	constexpr CIter	begin	() const	{ return data(); }
	constexpr  Iter	end		()			{ return data() + size(); }
	constexpr CIter	end		() const	{ return data() + size(); }

private:

	AX_INLINE void _checkBound			( Int i ) const { if( ! inBound(i) ) throw Error_IndexOutOfRange(); }
	AX_INLINE void	_debug_checkBound	( Int i ) const {
#ifdef AX_BUILD_CONFIG_Debug
		_checkBound(i);
#endif
	}	
};

template<class T> constexpr bool Type_IsIArray = false; 
template<class T> constexpr bool Type_IsIArray<IArray<T>> = true; 


template <class T>
constexpr void IArray<T>::append(const T& item) {
	auto oldSize = size();
	reserve(oldSize + 1);
	ax_call_constructor<T>(data() + oldSize, item);
	_storage.setSize(oldSize + 1);
}

template <class T>
constexpr void IArray<T>::append(T && item) {
	auto oldSize = size();
	reserve(oldSize + 1);
	ax_call_constructor<T>(data() + oldSize, AX_FORWARD(item));
	_storage.setSize(oldSize + 1);
}

} // namespace
