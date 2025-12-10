module;


export module AxCore.ScopedLock;

import AxCore.BasicType;

/* usage:
	auto ax_ = ax_lock(mutex)
	auto ax_ = ax_lock(mutex0, mutex1)
*/

export namespace ax::Thread {

class TryLockTag {};

template<class MUTEX>
class ScopedLock_ : public NonCopyable {
public:
	using Mutex = MUTEX;

	ScopedLock_() = default;
	ScopedLock_(ScopedLock_ && r) : _mutex(r._mutex) { r._mutex = nullptr; }

	class TryTag {};

	explicit ScopedLock_(MUTEX& mutex) { lock(mutex); }
	explicit ScopedLock_(const TryTag&, MUTEX& mutex) { tryLock(mutex); }

	~ScopedLock_() { unlock(); }

	AX_NODISCARD	explicit operator bool() const { return isLocked(); }

	AX_NODISCARD	bool isLocked() const { return _mutex; }

					void lock	(MUTEX& mutex) { if (_mutex != &mutex) { unlock(); mutex.lock(); _mutex = &mutex; } }
	AX_NODISCARD	bool tryLock(MUTEX& mutex) { return _tryLock(mutex); }

					void unlock	()	{ if (_mutex) { _mutex->unlock(); _mutex = nullptr; } }

					void detach	()	{ _mutex = nullptr; }

	AX_NODISCARD	MUTEX* mutex() { return _mutex; }
protected:

	AX_NODISCARD bool _tryLock(MUTEX& mutex) {
		unlock();
		if (mutex.tryLock()) {
			_mutex = &mutex;
			_mutex->lock();
			return true;
		}
		return false;
	}

	MUTEX* _mutex = nullptr;
};

template<class MUTEX>
class ScopedReadLock_ : public NonCopyable {
public:
	using Mutex = MUTEX;

	ScopedReadLock_() = default;
	ScopedReadLock_(ScopedReadLock_ && r) : _v(r._v) { r._v = nullptr; }
	explicit ScopedReadLock_(MUTEX& p) { lock(p); }

	~ScopedReadLock_() { unlock(); }

	void lock(MUTEX& p) { unlock(); _v = &p; _v->lockRead(); }
	void unlock()		{ if (_v) { _v->unlockRead(); _v = nullptr; } }

	void detach() { _v = nullptr; }

	MUTEX* mutex() { return _v; }
private:
	MUTEX* _v = nullptr;
};

template<class T0, class T1>
class ScopedLock2_ : public NonCopyable {
public:
	ScopedLock2_(ScopedLock2_ && r) : m0(ax_move(r.m0)), m1(ax_move(r.m1)) {}

	explicit ScopedLock2_(T0& t0, T1& t1) {
		if (&t0 < &t1) { //using address to decide the order
			m0.lock(t0);
			m1.lock(t1);
		}else{
			m1.lock(t1);
			m0.lock(t0);
		}
	}

	explicit ScopedLock2_(const TryLockTag&, T0& t0, T1& t1) {
		if (&t0 < &t1) { //using address to decide the order
			m0.tryLock(t0);
			m1.tryLock(t1);
		}else{
			m1.tryLock(t1);
			m0.tryLock(t0);
		}
	}

	explicit operator bool() const { return isLocked(); }

	bool isLocked() const { return m0.isLocked() && m1.isLocked(); }

	ScopedLock_<T0> m0;
	ScopedLock_<T1> m1;
};

template<class T0>           AX_NODISCARD inline ScopedLock_< T0>     lockScope(T0& t0)         { return ScopedLock_< T0    >(t0); }
template<class T0, class T1> AX_NODISCARD inline ScopedLock2_<T0, T1> lockScope(T0& t0, T1& t1) { return ScopedLock2_<T0, T1>(t0, t1); }

template<class T0>           AX_NODISCARD inline ScopedLock_< T0>     tryLookScope(T0& t0)         { return ScopedLock_< T0    >(TryLockTag(), t0); }
template<class T0, class T1> AX_NODISCARD inline ScopedLock2_<T0, T1> tryLookScope(T0& t0, T1& t1) { return ScopedLock2_<T0, T1>(TryLockTag(), t0, t1); }

} // namespace