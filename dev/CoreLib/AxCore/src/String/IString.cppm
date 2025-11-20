export module AxCore.IString;

export import AxCore.IArray;
export import AxCore.ZStrView;

export namespace ax {

template<class T>
class IString_ : public IArray<T, EArrayMode::String> {
	using Base = IArray<T, EArrayMode::String>;
public:
	IString_(T* data, Int initCap) : Base(data, initCap) {}
};

using IString   = IString_<Char>;
using IStringW  = IString_<CharW>;
using IString8  = IString_<Char8>;
using IString16 = IString_<Char16>;
using IString32 = IString_<Char32>;


} // namespace
