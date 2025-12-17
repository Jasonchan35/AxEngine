module;

export module AxCore.Scope;
export import AxCore.BasicType;

export namespace ax {

template<class T>
class ScopedValue {
public:
	AX_NODISCARD ScopedValue(T* p) { checkpoint(p); }
	AX_NODISCARD ScopedValue(T* p, const T& newValue) {
		checkpoint(p);
		if (p) *p = newValue;
	}

	AX_NODISCARD ScopedValue(ScopedValue&& r) noexcept {
		std::swap(_p, r._p);
		std::swap(_backup, r._backup);
	}

	~ScopedValue() { restore(); }

	void checkpoint(T* newPtr) {
		if (newPtr == _p) return;
		restore();
		_p = newPtr;
		if (newPtr) { _backup = *newPtr; }
	}

	void restore() {
		if (_p) {
			*_p = _backup;
			_p  = nullptr;
		}
	}

	void detach() { _p = nullptr; }

private:
	T  _backup;
	T* _p = nullptr;
};

struct ScopeEnterOnce : public NonCopyable {
	struct Scoped : public NonCopyable {
		AX_NODISCARD Scoped(Scoped && r) noexcept { std::swap(_p, r._p); }
		AX_NODISCARD Scoped(bool& entered) noexcept {
			if (!entered) {
				_p      = &entered;
				entered = true;
			}
		}

		~Scoped() noexcept { if (_p) *_p = false; }

		explicit operator bool() const { return _p != nullptr; }
		bool*    _p = nullptr;
	};

	AX_NODISCARD Scoped enter() { return Scoped(_entered); }
	bool _entered = false;
};

template<class LAMBDA>
class ScopedLambda : public NonCopyable {
public:
	AX_NODISCARD ScopedLambda(LAMBDA&& lambda)
		: _lambda(std::move(lambda))
		, _valid(true) {}
	AX_NODISCARD ScopedLambda(ScopedLambda&& r) noexcept {
		std::swap(_valid, r._valid);
		_lambda = std::move(r._lambda);
	}

	~ScopedLambda() { if (_valid) { _lambda(); } }

	void detach() {
		_valid  = false;
		_lambda = std::move(LAMBDA());
	}

private:
	LAMBDA _lambda;
	bool   _valid = false;
};

template<void (*FUNC)()>
class ScopedStaticFunc0 : public NonCopyable {
public:
	AX_NODISCARD ScopedStaticFunc0()
		: _valid(true) {}
	AX_NODISCARD ScopedStaticFunc0(ScopedStaticFunc0&& r) noexcept {
		std::swap(_valid, r._valid);
	}
	~ScopedStaticFunc0() { if (_valid) (*FUNC)(); }

private:
	bool _valid = false;
};

template<class PARAM0, void (*FUNC)(PARAM0)>
class ScopedStaticFunc1 : public NonCopyable {
public:
	AX_NODISCARD ScopedStaticFunc1(PARAM0&& param0)
		: _param0(param0), _valid(true) {}
	AX_NODISCARD ScopedStaticFunc1(ScopedStaticFunc1&& r) noexcept {
		std::swap(_valid, r._valid);
		std::swap(_param0, r._param0);
	}

	~ScopedStaticFunc1() { if (_valid) (*FUNC)(std::move(_param0)); }

private:
	PARAM0 _param0 = {};
	bool   _valid  = false;
};

template<class OWNER, void (OWNER::*FUNC)()>
class ScopedMemFunc0 : public NonCopyable {
public:
	AX_NODISCARD ScopedMemFunc0(OWNER* obj)
		: _obj(obj) {}
	AX_NODISCARD ScopedMemFunc0(ScopedMemFunc0&& r) noexcept {
		std::swap(_obj, r._obj);
	}
	~ScopedMemFunc0() { if (_obj) (_obj->*FUNC)(); }

private:
	OWNER* _obj = nullptr;
};

template<class OWNER, class PARAM0, void (OWNER::*FUNC)(PARAM0)>
class ScopedMemFunc1 : public NonCopyable {
public:
	AX_NODISCARD ScopedMemFunc1(OWNER* obj, PARAM0&& param0)
		: _obj(obj), _param0(param0) {}
	AX_NODISCARD ScopedMemFunc1(ScopedMemFunc1&& r) noexcept {
		std::swap(_obj, r._obj);
		std::swap(_param0, r._param0);
	}

	~ScopedMemFunc1() { if (_obj) (_obj->*FUNC)(_param0); }

private:
	OWNER* _obj    = nullptr;
	PARAM0 _param0 = {};
};


template<class DATA, void (*FUNC)()>
class ScopedStaticFuncProxy0 : public NonCopyable {
public:
	AX_NODISCARD ScopedStaticFuncProxy0(DATA && data)
		: _data(AX_FORWARD(data)), _valid(true) {}
	AX_NODISCARD ScopedStaticFuncProxy0(ScopedStaticFuncProxy0 && r) noexcept {
		std::swap(_data,  r._data);
		std::swap(_valid, r._valid);
	}
	~ScopedStaticFuncProxy0() { if (_valid) (*FUNC)(); }
	
	DATA& data()		{ return _data; }
	operator DATA&()	{ return _data; }
	DATA* operator->()	{ return &_data; }
	
private:
	DATA	_data;
	bool	_valid = false;
};

template<class DATA, class PARAM0, void (*FUNC)(PARAM0)>
class ScopedStaticFuncProxy1 : public NonCopyable {
public:
	AX_NODISCARD ScopedStaticFuncProxy1(DATA && data, PARAM0 && param0)
		: _data(AX_FORWARD(data)), _valid(true), _param0(AX_FORWARD(param0)) {}

	AX_NODISCARD ScopedStaticFuncProxy1(ScopedStaticFuncProxy1 && r) noexcept {
		std::swap(_data,   r._data);
		std::swap(_valid,  r._valid);
		std::swap(_param0, r._param0);
	}
	~ScopedStaticFuncProxy1() { if (_valid) (*FUNC)(std::move(_param0)); }
	
	DATA& data()		{ return _data; }
	operator DATA&()	{ return _data; }
	DATA* operator->()	{ return &_data; }
	
private:
	DATA	_data;
	bool	_valid = false;
	PARAM0	_param0;
};

template<class DATA, class OWNER, void (OWNER::*FUNC)()>
class ScopedMemFuncProxy0 : public NonCopyable {
public:
	AX_NODISCARD ScopedMemFuncProxy0(DATA && data, OWNER* owner)
		: _data(AX_FORWARD(data)), _owner(owner) {}

	AX_NODISCARD ScopedMemFuncProxy0(ScopedMemFuncProxy0 && r) noexcept {
		std::swap(_data,  r._data);
		std::swap(_owner, r._owner);
	}
	~ScopedMemFuncProxy0() { if (_owner) (_owner->*FUNC)(); }
	
	DATA& data()		{ return _data; }
	operator DATA&()	{ return _data; }
	DATA* operator->()	{ return &_data; }
	
private:
	DATA	_data;
	OWNER*	_owner = nullptr;
};

template<class DATA, class OWNER, class PARAM0, void (OWNER::*FUNC)(PARAM0)>
class ScopedMemFuncProxy1 : public NonCopyable {
public:
	AX_NODISCARD ScopedMemFuncProxy1(DATA && data, OWNER* owner, PARAM0 && param0)
		: _data(AX_FORWARD(data)), _owner(owner), _param0(AX_FORWARD(param0)) {}

	AX_NODISCARD ScopedMemFuncProxy1(ScopedMemFuncProxy1 && r) noexcept {
		std::swap(_data,   r._data);
		std::swap(_owner,  r._owner);
		std::swap(_param0, r._param0);
	}
	~ScopedMemFuncProxy1() { if (_owner) (_owner->*FUNC)(std::move(_param0)); }
	
	DATA& data()		{ return _data; }
	operator DATA&()	{ return _data; }
	DATA* operator->()	{ return &_data; }
	
private:
	DATA	_data;
	OWNER*	_owner = nullptr;
	PARAM0	_param0;
};

} // namespace