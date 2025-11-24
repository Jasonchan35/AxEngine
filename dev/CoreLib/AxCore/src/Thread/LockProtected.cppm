module;
#include "AxCore-pch.h"

export module AxCore.LockProtected;

import AxCore.BasicType;

export namespace ax::Thread {

template<class MUTEX, class DATA>
class LockProtected : public NonCopyable {
	using This = LockProtected;
public:
	using Mutex = MUTEX;
	using Data  = DATA;

	class ScopedLock : public Mutex::ScopedLock {
		using Base = typename Mutex::ScopedLock;
	public:
		ScopedLock(Mutex& mutex, Data* data) : Base(mutex), _data(data) {}
		ScopedLock(ScopedLock && r) : Base(std::move(r)), _data(r._data) { r._data = nullptr; }
		void unlock() { Base::unlock(); _data = nullptr; }

		operator Data*	() { return _data; }
		Data* operator->() { return _data; }
		Data*	    data() { return _data; }

		Mutex* mutex() { return Base::mutex(); }
	private:
		Data* _data = nullptr;
	};

	template<class... Args>
	LockProtected(Args&&... args) : _data(new(_dataBuffer) Data(AX_FORWARD(args)...)) {}
	~LockProtected() {
		auto lock = _mutex.scopedLock();
		_data->~Data();
		_data = nullptr;
	}

	AX_NODISCARD ScopedLock scopedLock() { return ScopedLock(_mutex, _data); }

	Mutex& mutex() { return _mutex; }

protected:
	Mutex	_mutex;
	Data*	_data = nullptr;
	Byte	_dataBuffer[ax_sizeof<Data>];
};

template<class MUTEX, class DATA>
class SharedLockProtected : public NonCopyable {
	using This = SharedLockProtected;
public:
	using Mutex = MUTEX;
	using Data  = DATA;

	class ScopedLock : public NonCopyable {
	public:
		ScopedLock(Mutex& mutex, Data* data) : _mutex(&mutex), _data(data) {
			mutex.lock();
		}

		ScopedLock(ScopedLock && r) : _mutex(r._mutex), _data(r._data) {
			r._mutex = nullptr;
			r._data = nullptr; 
		}

		~ScopedLock() { unlock(); }

		void unlock() {
			if (_mutex) {
				_mutex->unlock();
				_mutex = nullptr;
				_data  = nullptr;
			}
		}

		Data* operator->() { return _data; }
		Mutex* mutex() { return _mutex; }
	private:
		Mutex* _mutex = nullptr;
		Data* _data = nullptr;
	};

	class ScopedReadLock : public NonCopyable {
	public:
		ScopedReadLock(Mutex& mutex, Data& data) {
			_mutex = &mutex;
			_data = &data;
			mutex.lockRead();
		}

		ScopedReadLock(ScopedLock && r) : _mutex(r._mutex), _data(r._data) { 
			r._mutex = nullptr;
			r._data = nullptr; 
		}

		~ScopedReadLock() { unlock(); }

		void unlock() {
			if (_mutex) {
				_mutex->unlockRead();
				_mutex = nullptr;
				_data = nullptr;
			}
		}

		const Data* operator->() { return _data; }
		Mutex* mutex() { return _mutex; }
	private:
		Mutex* _mutex = nullptr;
		const Data* _data = nullptr;
	};

	template<class... Args>
	SharedLockProtected(Args&&... args) : _data(new(_dataBuffer) Data(AX_FORWARD(args)...)) {}

	~SharedLockProtected() {
		auto lock = _mutex.scopedLock();
		_data->~Data();
		_data = nullptr;
	}

	AX_NODISCARD ScopedLock		scopedLock()		{ return ScopedLock(    _mutex, _data); }
	AX_NODISCARD ScopedReadLock	scopedReadLock()	{ return ScopedReadLock(_mutex, _data); }

protected:
	Mutex	_mutex;
	Data*	_data = nullptr;
	Byte	_dataBuffer[ax_sizeof<Data>];
};

} // namespace