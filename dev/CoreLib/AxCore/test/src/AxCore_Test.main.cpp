#include "AxUnitTest.h"

import AxCore.UnitTest;

class AxCore_TestProgram : public ax::UnitTestProgram {
public:
	virtual void onRun() override {
		constexpr bool test_all = false;
		if constexpr (test_all) { 
			AX_TEST_GROUP(AxCore_Test)
		} else {
			AX_TEST_GROUP(Test_Rtti);
		}
	}
};

int main() {
	AxCore_TestProgram test;
	return test.run();
}
