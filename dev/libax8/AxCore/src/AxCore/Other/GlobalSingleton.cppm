module;
#include "AxCore-pch.h"

export module AxCore.GlobalSingleton;
import AxCore.LinkedList;
import AxCore.SpinLock;

export namespace ax {

class GlobalSingletonBase : public LinkedListNode<GlobalSingletonBase> {
	using This = GlobalSingletonBase;
public:
	class AllocatorInit {};

	virtual ~GlobalSingletonBase() {}
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
	template<class... Args>
	void _ctor(Args&&... args);

	char _buffer[sizeof(T)];
	bool _inited;
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

	Thread::InternalSpinLockProtected<Data> _data;
};

//---------------------

template<class T>
template<class... Args> inline
GlobalSingleton<T>::GlobalSingleton(Args&&... args) {
	_inited = true;
	ax_call_constructor(reinterpret_cast<T*>(_buffer), AX_FORWARD(args)...);

	GlobalSingletonManager::s_instance()->add(this);
}

template<class T>
inline GlobalSingleton<T>::~GlobalSingleton() {
	GlobalSingletonManager::s_instance()->shutdown();
}

template<class T> 
inline void GlobalSingleton<T>::callDestructor() {
	AX_ASSERT(_inited);
	ax_call_destructor(reinterpret_cast<T*>(_buffer));
	_inited = false;
}
} // namespace 