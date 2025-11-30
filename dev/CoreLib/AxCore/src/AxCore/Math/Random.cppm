module;

#include "AxCore-pch.h"

export module AxCore.Random;
export import AxCore.Math;

export namespace ax {

class RandomDevice {
public:
	void seed(Int s) { _generator.seed(static_cast<UInt>(s)); }

	// exclude max
	template<class T = Int> requires Type_IsInt<T>
	T getInt(T max) { return max <= 0 ? 0 : genIntRange(0, max - 1); }
	
	template<class T = Int> requires Type_IsInt<T>
	T genRange(const T& min, const T& max) {
		auto n = max - min + 1;
		if (n <= 0) return min;
		return static_cast<T>(_gen_u64() % static_cast<u64>(n)) + min;
	}

	template<class T = Int> requires Type_IsFloat<T>
	T genRange(const T& min, const T& max) {
		std::uniform_real_distribution<T> dis(min, max);
		return dis(_generator);
	}

private:
	std::random_device	_randDev;
	std::mt19937_64		_generator {_randDev()};

	AX_INLINE u64 _gen_u64() { return _generator(); }
};

struct Random {
	Random() = delete;
	
	// exclude max
	template<class T = Int> requires Type_IsInt<T>
	static T genInt(T max) { return s_device().getInt(max); }
	
	template<class T = Int>
	static T range(const T& min, const T& max) { return s_device().genRange(min, max); }

private:
	static RandomDevice& s_device() { static RandomDevice s; return s; }
};

} // namespace