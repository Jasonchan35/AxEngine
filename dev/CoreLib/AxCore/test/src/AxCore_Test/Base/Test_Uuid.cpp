#include "AxCore_Test-pch.h"
import AxCore_Test._PCH;

import AxCore.Uuid;

namespace ax {

class Test_Uuid : public UnitTestClass {
public:
	void test_case1() {
		auto uuid = Uuid::s_gen();
		AX_TEST_IF(uuid.isValid());
		return;
	}
	
};

} // namespace

void Test_Uuid() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Uuid::test_case1);
}
