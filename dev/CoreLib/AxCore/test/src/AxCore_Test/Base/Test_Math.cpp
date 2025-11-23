#include "AxUnitTest.h"

namespace ax {

class TestClase_Math : public UnitTestClass {
public:
	void test_case1() {
		Math::nextPow2(10);
	}
	
};

} // namespace

void Test_Math() {
	AX_TEST_RUN_CASE(ax::TestClase_Math::test_case1);
}
