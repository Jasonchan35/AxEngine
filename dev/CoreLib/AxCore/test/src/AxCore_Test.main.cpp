#include "AxUnitTest.h"

import AxCore.UnitTest;

class AxCore_TestProgram : public ax::UnitTestProgram {
public:
	virtual void onRun() override {
		constexpr bool test_all = true;
		if (test_all) {
			AX_TEST_GROUP(AxCore_Test)
		} else {
			AX_TEST_GROUP(Test_SPtr);
		}
	}
};

int main() {
	AxCore_TestProgram test;
	return test.run();
}
