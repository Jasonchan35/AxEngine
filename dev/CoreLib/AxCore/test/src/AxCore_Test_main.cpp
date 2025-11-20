#include "AxUnitTest.h"

import AxCore.UnitTest;

namespace ax {

class AxCore_TestProgram : public UnitTestProgram {
public:
	virtual void onRun() override {
		constexpr bool test_all = true;
		if constexpr(test_all) {
			// ALL
			AX_TEST_GROUP(AxCore_Test)
		} else {
			AX_TEST_GROUP(Test_Math);
		}
	}
};

} // namespace

int main() {
	ax::AxCore_TestProgram test;
	return test.run();
}
