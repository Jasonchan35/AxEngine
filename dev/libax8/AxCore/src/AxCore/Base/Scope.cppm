module;

export module AxCore.Scope;
export import AxCore.BasicType;

export namespace ax {

template<class LAMBDA>
class ScopeLambda : public NonCopyable {
public:
	AX_NODISCARD ScopeLambda(LAMBDA && lambda) : _valid(true), _lambda(std::move(lambda)) {}
	ScopeLambda(ScopeLambda && r) noexcept { std::swap(_valid, r._valid); _lambda = std::move(r._lambda); }
	~ScopeLambda() { if (_valid) { _lambda(); } }

	void detach() { _valid = false; _lambda = std::move(LAMBDA()); }
	
private:
	bool _valid = false;
	LAMBDA _lambda;
};

template<void (*FUNC)()>
class ScopeFunc0 : public NonCopyable {
public:
	AX_NODISCARD ScopeFunc0() : _valid(1) {}
	ScopeFunc0(ScopeFunc0 && r) noexcept { std::swap(_valid, r._valid); }
	~ScopeFunc0() { if (_valid) (*FUNC)(); }
private:
	u8 _valid = false;
};

template<class OBJ, void (OBJ::*FUNC)()>
class ScopeObjFunc0 : public NonCopyable {
public:
	AX_NODISCARD ScopeObjFunc0(OBJ* obj) : _obj(obj) {}
	ScopeObjFunc0(ScopeObjFunc0 && r) noexcept { std::swap(_obj, r._obj); }
	~ScopeObjFunc0() { if (_obj) (_obj->*FUNC)(); }
private:
	OBJ*	_obj = nullptr;
};

template<class PARAM0, void (*FUNC)(PARAM0)>
class ScopeFunc1 : public NonCopyable {
public:
	AX_NODISCARD ScopeFunc1(PARAM0 && param0) : _valid(1), _param0(param0) {}
	ScopeFunc1(ScopeFunc1 && r) noexcept { std::swap(_valid, r._valid); std::swap(_param0, r._param0); }
	~ScopeFunc1() { if (_valid) (*FUNC)(std::move(_param0)); }
private:
	u8		_valid = false;
	PARAM0	_param0 = {};
};

template<class OBJ, class PARAM0, void (OBJ::*FUNC)(PARAM0)>
class ScopeObjFunc1 : public NonCopyable {
public:
	AX_NODISCARD ScopeObjFunc1(OBJ* obj, PARAM0 && param0) : _obj(obj), _param0(param0) {}
	ScopeObjFunc1(ScopeObjFunc1 && r) noexcept { std::swap(_obj, r._obj); std::swap(_param0, r._param0); }
	~ScopeObjFunc1() { if (_obj) (_obj->*FUNC)(_param0); }
private:
	OBJ*	_obj = nullptr;
	PARAM0	_param0 = {};
};

template<class T>
class ScopeValue {
public:
	AX_NODISCARD ScopeValue(T* p) { save(p); }
	AX_NODISCARD ScopeValue(T* p, const T& newValue) { save(p); if(p) *p = newValue; }
	AX_NODISCARD ScopeValue(ScopeValue && r) noexcept { std::swap(_p, r._p); std::swap(_backup, r._backup); }
	~ScopeValue() { restore(); }

	void save(T* newPtr) {
		if (newPtr == _p) return;
		restore();
		_p = newPtr;
		if (newPtr) { _backup = *newPtr; }
	}

	void restore() { if (_p) { *_p = _backup; _p = nullptr; } }
	void detach() { _p = nullptr; }

private:
	T  _backup;
	T* _p = nullptr;
};

struct ScopeEnterOnce : public NonCopyable {
	struct Scoped : public NonCopyable {
		Scoped(Scoped && r) noexcept	{ std::swap(_p, r._p); }
		Scoped(bool& entered) noexcept	{ if (!entered) { _p = &entered; entered = true; } }
		~Scoped() noexcept { if (_p) *_p = false; }

		explicit operator bool() const { return _p != nullptr; }
		bool* _p = nullptr;
	};
	AX_NODISCARD Scoped enter() { return Scoped(_entered); }
	bool _entered = false;
};


template<class DATA, class OWNER, void (OWNER::*FUNC)()>
class ScopeDataProxy0 : public NonCopyable {
public:
	AX_NODISCARD ScopeDataProxy0(DATA && data, OWNER* owner)
		: _data(AX_FORWARD(data)), _owner(owner) {}

	ScopeDataProxy0(ScopeDataProxy0 && r) noexcept {
		std::swap(_data,  r._data);
		std::swap(_owner, r._owner);
	}
	~ScopeDataProxy0() { if (_owner) (_owner->*FUNC)(); }
	
	DATA& data()		{ return _data; }
	operator DATA&()	{ return _data; }
	DATA* operator->()	{ return &_data; }
	
private:
	DATA	_data;
	OWNER*	_owner = nullptr;
};

template<class DATA, class OWNER, class PARAM0, void (OWNER::*FUNC)(PARAM0)>
class ScopeDataProxy1 : public NonCopyable {
public:
	AX_NODISCARD ScopeDataProxy1(DATA && data, OWNER* owner, PARAM0 && param0)
		: _data(AX_FORWARD(data)), _owner(owner), _param0(AX_FORWARD(param0)) {}

	ScopeDataProxy1(ScopeDataProxy1 && r) noexcept {
		std::swap(_data,   r._data);
		std::swap(_owner,  r._owner);
		std::swap(_param0, r._param0);
	}
	~ScopeDataProxy1() { if (_owner) (_owner->*FUNC)(std::move(_param0)); }
	
	DATA& data()		{ return _data; }
	operator DATA&()	{ return _data; }
	DATA* operator->()	{ return &_data; }
	
private:
	DATA	_data;
	OWNER*	_owner = nullptr;
	PARAM0	_param0;
};

template<class DATA, class OWNER, class PARAM0, class PARAM1, void (OWNER::*FUNC)(PARAM0, PARAM1)>
class ScopeDataProxy2 : public NonCopyable {
public:
	AX_NODISCARD ScopeDataProxy2(DATA && data, OWNER* owner, PARAM0 && param0, PARAM1 && param1)
		: _data(AX_FORWARD(data)), _owner(owner), _param0(AX_FORWARD(param0)), _param1(AX_FORWARD(param1)) {}

	ScopeDataProxy2(ScopeDataProxy2 && r) noexcept {
		std::swap(_data,   r._data);
		std::swap(_owner,  r._owner);
		std::swap(_param0, r._param0);
		std::swap(_param1, r._param1);
	}
	~ScopeDataProxy2() { if (_owner) (_owner->*FUNC)(std::move(_param0), std::move(_param1)); }

	DATA& data()		{ return _data; }
	operator DATA&()	{ return _data; }
	DATA* operator->()	{ return &_data; }
	
private:
	DATA	_data;
	OWNER*	_owner = nullptr;
	PARAM0	_param0;
	PARAM1	_param1;
};

} // namespace