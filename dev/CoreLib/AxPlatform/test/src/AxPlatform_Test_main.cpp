#include "AxUnitTest.h"

import AxPlatform.UnitTest;

import AxPlatform_Test.Test_LinkedList;


class AxPlatform_TestProgram : public ax::UnitTestProgram {
public:
	virtual void onRun() override {
		constexpr bool test_all = false;
		if constexpr(test_all) {
			// ALL
			AX_TEST_GROUP(AxPlatform_Test)
		} else {
//			AX_TEST_GROUP(Test_Math);
			AX_TEST_RUN_CASE(ax::Test_LinkedList::test_case1)
		}
	}
};

int main() {
	AxPlatform_TestProgram test;
	return test.run();
}
