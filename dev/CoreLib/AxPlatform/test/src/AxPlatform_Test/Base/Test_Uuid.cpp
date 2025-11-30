#include "AxUnitTest.h"

import AxPlatform.Uuid;

namespace ax {

class Test_Uuid : public UnitTestClass {
public:
	void test_case1() {
		auto uuid = Uuid::s_gen();
		AX_TEST_CHECK(uuid.isValid());
		return;
	}
	
};

} // namespace

void Test_Uuid() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Uuid::test_case1);
}
