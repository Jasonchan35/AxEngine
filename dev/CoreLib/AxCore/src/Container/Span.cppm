export module AxCore.Span;


export import AxCore.BasicType;

export namespace ax {

template<class T>
class MutSpan {
	// copyable
public:

protected:
	T*	_data = nullptr;
	Int _size = 0;
};

} // namespace
