#include "AxUnitTest.h"

import AxCore.Dict;

namespace ax {

class Test_Dict : public UnitTestClass {
public:
	void test_case1() {
		
	}
};

} // namespace

void Test_Dict() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Dict::test_case1)
}

