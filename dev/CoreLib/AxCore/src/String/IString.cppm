export module AxCore.IString;

export import AxCore.IArrayStorage;
export import AxCore.ZStrView;

export namespace ax {

template<class T> class IString_;
using IString   = IString_<Char>;
using IStringW  = IString_<CharW>;
using IString8  = IString_<Char8>;
using IString16 = IString_<Char16>;
using IString32 = IString_<Char32>;

template<class T>
class IString_ : public IArrayStorage<T> {
	using Base = IArrayStorage<T>;
	using Base::_storage;	
protected:
	IString_(T* data, Int initCap) : Base(data, initCap) {}

public:
	constexpr T*  data() { return _storage.data(); }
	constexpr Int size() const { return _storage.size(); }
	constexpr Int capacity() const { return _storage.capacity(); }
	
	constexpr void clear() { Base::_storageClear(); }
	constexpr void clearAndFree() { Base::_storageClearAndFree(); }

	constexpr void reserve(Int newCapacity) { Base::_storageReserve(newCapacity); }
	
	template<class... Args>
	constexpr void resize(Int newSize, Args&&... args) { Base::_storageResize(newSize, AX_FORWARD(args)...); }
	
	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()			{ return data(); }
	constexpr CIter	begin	() const	{ return data(); }
	constexpr  Iter	end		()			{ return data() + size(); }
	constexpr CIter	end		() const	{ return data() + size(); }	
};

} // namespace
