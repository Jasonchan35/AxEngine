#include "AxUnitTest.h"

namespace ax {

class Test_Enum : public UnitTestClass {
public:
	void test_case1() {
		
	}
};

} // namespace

void Test_Enum() {
	AX_TEST_RUN_CASE(ax::Test_Enum::test_case1)
}

