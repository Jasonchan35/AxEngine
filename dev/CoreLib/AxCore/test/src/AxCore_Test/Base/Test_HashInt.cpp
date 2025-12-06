import AxCore_Test._PCH;

import AxCore.HashInt;

namespace ax {

class Test_HashInt : public UnitTestClass {
public:
	void test_case1() {
		constexpr auto inputData = StrView("Testing1234");
		{
			constexpr	HashInt const_fnv1a_64_result = HashInt::s_make(inputData);
						HashInt       fnv1a_64_result = HashInt::s_make(inputData);
			u64 k = 0x5e41ec79612fa7f3ull;
			AX_TEST_IF(const_fnv1a_64_result.value == k);
			AX_TEST_IF(      fnv1a_64_result.value == k);
		}
	}
};

} // namespace

void Test_HashInt() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_HashInt::test_case1)
}

