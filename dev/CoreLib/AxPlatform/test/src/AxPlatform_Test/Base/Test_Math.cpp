#include "AxUnitTest.h"

namespace ax {

class Test_Math : public UnitTestClass {
public:
	void test_case1() {
		AX_TEST_EQ(Math::nextPow2(10), 16);
	}
	
};

} // namespace

void Test_Math() {
	AX_TEST_RUN_CASE(ax::Test_Math::test_case1);
}
