#include "AxUnitTest.h"

import AxPlatform.UnitTest;

class AxPlatform_TestProgram : public ax::UnitTestProgram {
public:
	virtual void onRun() override {
		constexpr bool test_all = true;
		if constexpr(test_all) {
			// ALL
			AX_TEST_GROUP(AxPlatform_Test)
		} else {
			AX_TEST_GROUP(Test_Math);
		}
	}
};

int main() {
	AxPlatform_TestProgram test;
	return test.run();
}
