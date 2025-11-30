#include "AxUnitTest.h"

import AxCore.HashInt;

namespace ax {

class Test_HashInt : public UnitTestClass {
public:
	void test_case1() {
		constexpr auto inputData = StrView("Testing1234");
		{
			constexpr	HashInt32 const_fnv1a_32_result = HashInt32::s_make(inputData);
						HashInt32       fnv1a_32_result = HashInt32::s_make(inputData);
			u32 k = 0x9b8551d3;
			AX_TEST_CHECK(      fnv1a_32_result.value == k);
			AX_TEST_CHECK(const_fnv1a_32_result.value == k);
		}
		{
			constexpr	HashInt64 const_fnv1a_64_result = HashInt64::s_make(inputData);
						HashInt64       fnv1a_64_result = HashInt64::s_make(inputData);
			u64 k = 0x5e41ec79612fa7f3ull;
			AX_TEST_CHECK(const_fnv1a_64_result.value == k);
			AX_TEST_CHECK(      fnv1a_64_result.value == k);
		}
	}
};

} // namespace

void Test_HashInt() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_HashInt::test_case1)
}

