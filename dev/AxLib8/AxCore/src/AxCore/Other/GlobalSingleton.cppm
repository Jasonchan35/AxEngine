module;


export module AxCore.GlobalSingleton;
import AxCore.LinkedList;
import AxCore.SpinLock;

export namespace ax {

class GlobalSingletonBase : public LinkedListNode<GlobalSingletonBase> {
	using This = GlobalSingletonBase;
public:
	class AllocatorInit {};

	virtual ~GlobalSingletonBase() = default;
	virtual void callDestructor() {}
};

//! Guarantee the destructor order in global scope
template<class T>
class GlobalSingleton : public GlobalSingletonBase {
public:
	template<class... Args>
	GlobalSingleton(Args&&... args);
	virtual ~GlobalSingleton() override;

	T* ptr() { return reinterpret_cast<T*>(_buffer); }
	operator T* () { return ptr(); }

	virtual void callDestructor() override;

private:
	AX_VC_WARNING_PUSH_AND_DISABLE(4324) //  Warning C4324 : structure was padded due to alignment specifier
	AX_ALIGNAS(T) Byte _buffer[AX_SIZEOF(T)];
	AX_VC_WARNING_POP()
	
	bool _initialized = false;
};

class GlobalSingletonManager : public NonCopyable {
public:
	using List = LinkedList<GlobalSingletonBase>;

	void add(GlobalSingletonBase* p);

	static GlobalSingletonManager* s_instance();

	void shutdown();
	~GlobalSingletonManager() { shutdown(); }

private:
	struct Data {
		List list;
	};

	InternalSpinLockProtected<Data> _data;
};

//---------------------

template<class T>
template<class... ARGS> inline
GlobalSingleton<T>::GlobalSingleton(ARGS&&... args)
	: _initialized(true)
{
	ax_call_constructor(reinterpret_cast<T*>(_buffer), AX_FORWARD(args)...);
	GlobalSingletonManager::s_instance()->add(this);
}

template<class T>
inline GlobalSingleton<T>::~GlobalSingleton() {
	GlobalSingletonManager::s_instance()->shutdown();
}

template<class T> 
inline void GlobalSingleton<T>::callDestructor() {
	AX_ASSERT(_initialized);
	ax_call_destructor(reinterpret_cast<T*>(_buffer));
	_initialized = false;
}
} // namespace 