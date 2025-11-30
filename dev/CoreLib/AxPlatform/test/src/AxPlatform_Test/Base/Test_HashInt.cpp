#include "AxUnitTest.h"

import AxPlatform.HashInt;

namespace ax {

class Test_HashInt : public UnitTestClass {
public:
	void test_case1() {
		constexpr auto inputData = StrView("Testing1234");
		{
			constexpr HashInt32 fnv1a_32_result = HashUtil::compute32(inputData);
			AX_TEST_CHECK(fnv1a_32_result.value == 0xa5dfcf29u);
		}
		{
			constexpr HashInt64 fnv1a_64_result = HashUtil::compute64(inputData);
			AX_TEST_CHECK(fnv1a_64_result.value == 0x59acc54023fa61e9ull);
		}
	}
};

} // namespace

void Test_HashInt() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_HashInt::test_case1)
}

