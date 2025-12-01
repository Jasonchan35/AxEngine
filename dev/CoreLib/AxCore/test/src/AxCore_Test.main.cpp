#include "AxUnitTest.h"

import AxCore.UnitTest;

class AxCore_TestProgram : public ax::UnitTestProgram {
public:
	virtual void onRun() override {
		// AX_TEST_GROUP(Test_LinkedList);

		
		// ALL
		AX_TEST_GROUP(AxCore_Test)
	}
};

int main() {
	AxCore_TestProgram test;
	return test.run();
}
