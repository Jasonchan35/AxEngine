#include "AxUnitTest.h"

namespace ax {

struct Test_Math : public UnitTestCase {
	void test1() {
		Math::nextPow2(10);
	}

	void run() {
		test1();
	}
};

} // namespace

void Test_Math() { ax::Test_Math().run(); }
