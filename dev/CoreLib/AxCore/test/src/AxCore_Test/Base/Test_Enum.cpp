#include "AxUnitTest.h"

namespace ax {

class TestClass_Enum : public UnitTestClass {
public:
	void test_case1() {
		
	}
};

} // namespace

void Test_Enum() {
	AX_TEST_RUN_CASE(ax::TestClass_Enum::test_case1)
}

