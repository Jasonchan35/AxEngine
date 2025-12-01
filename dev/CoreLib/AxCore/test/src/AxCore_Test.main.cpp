#include "AxUnitTest.h"

import AxCore.UnitTest;

class AxCore_TestProgram : public ax::UnitTestProgram {
public:
	virtual void onRun() override {
		//---- ALL ----
		// AX_TEST_GROUP(AxCore_Test)
		//--------
		AX_TEST_GROUP(Test_BasicType);
	}
};

int main() {
	AxCore_TestProgram test;
	return test.run();
}
