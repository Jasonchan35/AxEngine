module;

#include "AxCore-pch.h"

export module AxCore.Random;
export import AxCore.Math;

export namespace ax {

class RandomDevice {
public:
	void seed(Int s) { _generator.seed(static_cast<UInt>(s)); }

	// exclude max
	template<class T = Int> requires Type_AnyInt<T>
	T getInt(T max) { return max <= 0 ? 0 : getRange(0, max - 1); }
	
	template<class T = Int> requires Type_AnyInt<T>
	T getRange(const T& min, const T& max) {
		auto n = max - min;
		if (n <= 0) return min;
		return static_cast<T>(_gen_u64() % static_cast<u64>(n)) + min;
	}

	template<class T = Int> requires Type_AnyFloat<T>
	T getRange(const T& min, const T& max) {
		std::uniform_real_distribution<T> dis(min, max);
		return dis(_generator);
	}

	static RandomDevice& s_default() { static RandomDevice s; return s; }
		
private:
	std::random_device	_randDev;
	std::mt19937_64		_generator {_randDev()};

	AX_INLINE u64 _gen_u64() { return _generator(); }
};

struct Random {
	// exclude max
	template <class T = Int> requires Type_AnyInt<T>
	static T getInt(T max, RandomDevice& dev = RandomDevice::s_default()) { return dev.getInt(max); }

	template <class T = Int>
	static T getRange(const T& min, const T& max, RandomDevice& dev = RandomDevice::s_default()) { return dev.getRange(min, max); }
};

}; // namespace ax::Random
