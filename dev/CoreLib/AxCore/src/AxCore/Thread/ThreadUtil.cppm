module;
#include "AxCore-pch.h"

export module AxCore.ThreadUtil;

import AxCore.Format;
import AxCore.TimeDuration;

export namespace ax::Thread {

#if AX_OS_WINDOWS

using ThreadAffinityMask = DWORD_PTR;

struct ThreadId_Native {
	constexpr ThreadId_Native() = default;
	constexpr ThreadId_Native(HANDLE handle_, DWORD id_) : handle(handle_), id(id_) {}

	operator HANDLE() const { return handle; }
	
	bool operator==(const ThreadId_Native& r) const { return handle == r.handle && id == r.id; }
	bool operator!=(const ThreadId_Native& r) const { return !operator==(r); }

	HANDLE	handle = nullptr;
	DWORD	id = 0;
};

#elif AX_OS_MACOSX || AX_OS_IOS

using ThreadAffinityMask = integer_t;

struct ThreadId_Native {
	constexpr ThreadId_Native() = default;
	constexpr ThreadId_Native(pthread_t id_) : id(id_) {}

	template<class CH> void onFmt(FmtCtx_<CH>& ctx) const { ctx << static_cast<const void*>(id); }

	bool operator==(const ThreadId_Native& r) const { return id == r.id; }
	bool operator!=(const ThreadId_Native& r) const { return !operator==(r); }

	pthread_t	id = nullptr; // it's pointer on Mac
};

#else

using ThreadAffinityMask = cpu_set_t;

struct ThreadId_Native {
	constexpr ThreadId_Native() = default;
	constexpr ThreadId_Native(pthread_t id_) : id(id_) {}

	template<class CH> void onFmt(FmtCtx_<CH>& ctx) const { ctx << static_cast<u64>(id); }

	bool operator==(const ThreadId_Native& r) const { return id == r.id; }
	bool operator!=(const ThreadId_Native& r) const { return !operator==(r); }

	pthread_t	id = 0;  // it's integer on Linux
};

#endif

constexpr ThreadId_Native ThreadId_Native_kNull = ThreadId_Native();

class ThreadId {
public:
	using NativeHandle = ThreadId_Native;

	void reset() { _v = ThreadId_Native_kNull; }

	static ThreadId s_current();

	explicit operator bool() const { return _v != ThreadId_Native_kNull; }
	bool operator==(const ThreadId& r) const { return _v == r._v; }
	bool operator!=(const ThreadId& r) const { return _v != r._v; }

	void setToCurrentThread() { *this = s_current(); }
	bool isCurrentThread();

	NativeHandle nativeHandle() { return _v; }

protected:
	ThreadId_Native _v = ThreadId_Native_kNull;
};

struct ThreadUtil {
	ThreadUtil() = delete;

#if 1 // C++11
	static AX_INLINE void yield() { ::std::this_thread::yield(); }

#elif AX_OS_WINDOWS
	static AX_INLINE void yield() { ::YieldProcessor(); }

#elif AX_OS_LINUX || AX_OS_IOS || AX_OS_MACOSX
	static AX_INLINE void yield() { ::cpu_relax(); }

#elif AX_COMPILER_GCC
	static AX_INLINE void yield() { __asm__("pause"); }

#elif 1// pthread
	static AX_INLINE void yield() { ::pthread_yield(void); }
#else
	#error
#endif

	static void sleep(Nanoseconds nanoseconds) {
		::std::this_thread::sleep_for(std::chrono::duration<Int, std::nano>(nanoseconds.value));
	}

	static void sleep(Microseconds microseconds) {
		::std::this_thread::sleep_for(std::chrono::duration<Int, std::micro>(microseconds.value));
	}

	static void sleep(Milliseconds milliseconds) {
		::std::this_thread::sleep_for(std::chrono::duration<Int, std::milli>(milliseconds.value));
	}

	static void sleep(Seconds seconds) {
		::std::this_thread::sleep_for(std::chrono::duration<Int>(seconds.value));
	}
};

} // namespace