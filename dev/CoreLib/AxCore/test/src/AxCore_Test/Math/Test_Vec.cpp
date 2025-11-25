#include "AxUnitTest.h"

import AxCore.Vec4;

namespace ax {

class Test_Vec : public UnitTestClass {
public:
	void test_case1() {
		Vec4f a(1, 2, 3, 4);
		Vec4f b(2, 3, 4, 5);
		auto c = a + b;
		AX_TEST_ALMOST_EQ(c, Vec4f(3,5,7,9));
		AX_TEST_NOT_ALMOST_EQ(c, Vec4f(3,9,7,9));
		AX_TEST_NOT_ALMOST_EQ(c, Vec4f(3,1,7,9));

		auto t = Math::epsilon<Vec4f>;
	}
};

void Test_Vec_Func() {
	AX_TEST_RUN_CASE(Test_Vec::test_case1)
}

} // namespace

void Test_Vec() { ax::Test_Vec_Func(); }
