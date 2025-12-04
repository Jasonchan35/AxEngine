module;
#include "AxCore-pch.h"
export module AxCore.Delegate;
export import AxCore.WPtr;

export namespace ax {

template<class FuncSig> class Delegate_;

template<class RETURN_TYPE, class... ARGS>
class Delegate_<RETURN_TYPE (ARGS...)> : public NonCopyable {
	using This = Delegate_;
public:
	using ReturnType	= RETURN_TYPE;	
						using StaticFunc	= ReturnType (*)(ARGS...);
	template<class OBJ>	using MemFunc		= ReturnType (OBJ::*)(ARGS...);


	Delegate_() = default;
	Delegate_(const Delegate_& r) { if (r._functor) r._functor->copyTo(*this); }
	Delegate_(Delegate_ && r) { operator=(std::move(r)); }

	~Delegate_() { unbindAll(); }

							// global or class static function	
	        void   bindStatic(StaticFunc func)		{ _bindStatic(func); }
	static	This s_bindStatic(StaticFunc func)		{ Delegate_ d; d.bindStatic(func); return d; }
	
	template<class OBJ>		       void   bindWPtr(SPtr<OBJ>& obj, MemFunc<OBJ> func)	{ _bindObj<WPtrFunctor<OBJ, MemFunc<OBJ>>>(obj.ptr(), func); }
	template<class OBJ>		static This s_bindWPtr(SPtr<OBJ>& obj, MemFunc<OBJ> func)	{ Delegate_ d; d.bindWPtr(obj, func); return d; }
	template<class OBJ>		       void   bindUnowned(OBJ*    obj, MemFunc<OBJ> func)	{ _bindObj<UnownedFunctor<OBJ, MemFunc<OBJ>>>(obj, func); }
	template<class OBJ>		static This s_bindUnowned(OBJ*    obj, MemFunc<OBJ> func)	{ Delegate_ d; d.bindUnowned(obj, func); return d; }
							// beware lambda capture variable life cycle
	template<class LAMBDA>	       void   bindLambda(LAMBDA func)						{ _bindLambda(func); }
	template<class LAMBDA>	static This s_bindLambda(LAMBDA func)						{ _bindLambda(func); }

	void unbindAll() { _unbindAll(); }

	AX_INLINE Opt<RETURN_TYPE>	invoke(ARGS... args) { return _invokeValid(AX_FORWARD(args)...); }

	AX_INLINE bool valid() const { return _functor != nullptr; }
	AX_INLINE explicit operator bool () const { return valid(); }

	void operator=(const Delegate_ & r) {
		unbindAll();
		if (r._functor) {
			r._functor->copyTo(*this);
		}
	}

	AX_INLINE Opt<RETURN_TYPE> _invokeValid(bool& outCalled, ARGS... args) {
		if (!_functor) return RETURN_TYPE();
		return _functor->onInvoke(AX_FORWARD(args)...);
	}

private:
	enum class Type {
		Unknown,
		StaticFunctor,
		LambdaFunctor,
		WPtrFunctor,
		UnownedFunctor,
	};

	struct Functor {
		Functor(Type type) : _type(type) {}
		virtual ~Functor() {}
		virtual bool valid() const = 0;
		virtual bool hasObject(void* p) const = 0;
		virtual void copyTo(Delegate_& de) = 0;

		virtual Opt<RETURN_TYPE> onInvoke(ARGS... args) = 0;

		template<class OBJ, class FUNC>
		Opt<RETURN_TYPE> doObjInvoke(OBJ* obj, FUNC func, ARGS... args) {
			if (!obj) return std::nullopt;

			if constexpr(std::is_same_v<FUNC, MemFunc<OBJ>>) {
				return (obj->*func)(AX_FORWARD(args)...);
			} else {
				static_assert(false);
				return std::nullopt;
			}
		}

		Type _type = Type::Unknown;
	};

	struct StaticFunctor : public Functor {
		using FUNC = StaticFunc;
		FUNC	_func;

		StaticFunctor(FUNC func) : Functor(Type::StaticFunctor), _func(func) {}
		virtual bool valid() const override				{ return true; }
		virtual bool hasObject(void* p) const override	{ return false; }
		virtual void copyTo(Delegate_& de) override	{ de._bindStatic(_func); }
		virtual Opt<RETURN_TYPE> onInvoke(ARGS... args) override { return _func(AX_FORWARD(args)...); }
	};

	template<class LAMBDA>
	struct LambdaFunctor : public Functor {
		using FUNC = LAMBDA;
		FUNC _func;

		LambdaFunctor(FUNC func) : Functor(Type::LambdaFunctor), _func(func) {}
		virtual bool valid() const override				{ return true; }
		virtual bool hasObject(void* p) const override	{ return false; }
		virtual void copyTo(Delegate_& de) override	{ de.bindLambda(_func); }
		virtual Opt<RETURN_TYPE> onInvoke(ARGS... args) override { return _func(AX_FORWARD(args)...); } 
	};

	template<class OBJ, class FUNC>
	struct UnownedFunctor : public Functor {
		OBJ*	_obj  = nullptr;
		FUNC	_func = nullptr;

		UnownedFunctor(OBJ* obj, FUNC func) : Functor(Type::UnownedFunctor), _obj(obj), _func(func) {}

		virtual bool valid() const override				{ return _obj != nullptr; }
		virtual bool hasObject(void* p) const override	{ return _obj == p; }
		virtual void copyTo(Delegate_& de) override	{ if (_obj) de.template _bindObj<UnownedFunctor>(_obj, _func); }
		virtual Opt<RETURN_TYPE> onInvoke(ARGS... args) override { return Functor::doObjInvoke(_obj, _func, AX_FORWARD(args)...); }
	};

	template<class OBJ, class FUNC>
	struct WPtrFunctor : public Functor {
		WPtr<OBJ>	_weak;
		FUNC		_func = nullptr;

		WPtrFunctor(OBJ* obj, FUNC func) : Functor(Type::WPtrFunctor), _weak(obj), _func(func) {}

		SPtr<      OBJ> obj()		{ return _weak.getSPtr(); }
		SPtr<const OBJ> obj() const	{ return _weak.getSPtr(); }

		virtual bool valid() const override				{ auto sp = obj(); return sp.ptr() != nullptr; }
		virtual bool hasObject(void* p) const override	{ auto sp = obj(); return sp.ptr() == p; }
		virtual void copyTo(Delegate_& de) override	{ if (auto sp = obj()) de._bindObj<WPtrFunctor>(sp.ptr(), _func); }

		virtual Opt<RETURN_TYPE> onInvoke(ARGS... args) override { return Functor::doObjInvoke(obj().ptr(), _func, AX_FORWARD(args)...); }
	};

	void _bindStatic(StaticFunc func) {
		unbindAll();
		static_assert(sizeof(_functorLocalBuf) >= sizeof(StaticFunctor), "_functorLocalBuf is too small");
		_functor = new(_functorLocalBuf) StaticFunctor(func);
	}

	template<class FUNCTOR, class OBJ, class FUNC>
	void _bindObj(OBJ* obj, FUNC func) {
		static_assert(std::is_same_v<MemFunc<OBJ>, FUNC>);
		unbindAll();
		static_assert(sizeof(_functorLocalBuf) >= sizeof(FUNCTOR), "_functorLocalBuf is too small");
		_functor = new(_functorLocalBuf) FUNCTOR(obj, func);
	}

	template<class LAMBDA>
	void _bindLambda(LAMBDA func) {
		static_assert(std::is_invocable_v<LAMBDA, ARGS...>);
		unbindAll();
		static_assert(sizeof(_functorLocalBuf) >= sizeof(LambdaFunctor<LAMBDA>), "_functorLocalBuf is too small");
		_functor = new(_functorLocalBuf) LambdaFunctor<LAMBDA>(func);
	}

	template<class FUNCTOR, class FUNC> 
	bool _hasFunc(FUNC func) {
		return static_cast<FUNCTOR*>(_functor)->_func == func;
	}

	template<class OBJ, class FUNC>
	bool _hasBinding(OBJ* obj, FUNC func) {
		if (!_functor) return false;
		if (!_functor->hasObject(obj)) return false;

		switch (_functor->_type) {
			case Type::StaticFunctor	: return _hasFunc<StaticFunctor		>(func);
			case Type::LambdaFunctor	: return _hasFunc<LambdaFunctor		>(func);
			case Type::WPtrFunctor		: return _hasFunc<WPtrFunctor		>(func);
			case Type::UnownedFunctor	: return _hasFunc<UnownedFunctor	>(func);
			default: AX_ASSERT(false); return false;
		}
	}

	template<class OBJ>
	bool _hasObject(OBJ* obj) {
		if (!obj) return false;
		if (!_functor) return false;
		return _functor->hasObject(obj);
	}

	void _unbindAll() {
		if (_functor) {
			ax_call_destructor(_functor);
			_functor = nullptr;
		}
	}

	static constexpr Int kFunctorlocalBufSize = sizeof(void*) * 16;
	Functor*	_functor = nullptr;
	char		_functorLocalBuf[kFunctorlocalBufSize];
};

} // namespace 