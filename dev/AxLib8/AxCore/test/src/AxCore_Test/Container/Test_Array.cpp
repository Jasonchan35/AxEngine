import AxCore_Test.PCH;
#include "AxUnitTest.h"

namespace ax {

class Test_Array : public UnitTestClass {
public:
	void test_case1() {
		// constexpr Array<u8> u8arr;
		
		Array<Int> arr;
		for (Int i = 0; i < 6; ++i) {
			arr.append(i * 10);
		}

		const auto& const_arr = arr;
		
		auto* p = const_arr.find(30);
		if (AX_TEST_IF(p != nullptr)) {
			auto index = const_arr.getIndexFromElementPtr(p);
			if (AX_TEST_IF(index)) {
				AX_TEST_EQ(index.value(), 3);
			}
			AX_TEST_EQ(*p, 30);
		}
	}
};

} // namespace

void Test_Array() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Array::test_case1)
}

