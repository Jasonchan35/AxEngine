export module AxCore.ComPtr;

#include "AxBase.h"
import AxCore.BasicType;

namespace ax {

template<class T>
class ComPtr : public NonCopyable {
public:
	ComPtr() = default;
	ComPtr(const ComPtr& r) { ref(r.ptr()); }
	ComPtr( ComPtr && r ) noexcept {
		_p = r._p;
		r._p = nullptr;
	}

	~ComPtr() noexcept { unref(); }

	T* operator->() noexcept { return _p; }
	  operator T*() noexcept { return _p; }

	void ref(T* p) { 
		if (p == _p) return; 
		unref(); 
		_p = p; 
		if (_p) _p->AddRef();
	}
	void unref() noexcept {
		if (_p) {
			_p->Release();
			_p = nullptr;
		}
	}

	T** ptrForInit() noexcept { unref(); return &_p; }

		  T* ptr() noexcept			{ return _p; }
	const T* ptr() const noexcept	{ return _p; }

	T* detach() { T* o = _p; _p = nullptr; return o; }
private:
	T*	_p = nullptr;
};



} // namespace ax