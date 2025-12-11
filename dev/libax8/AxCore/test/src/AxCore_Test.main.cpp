// module;  // - main() not allowed to under any module

#include "AxUnitTest.h"
import AxCore.UnitTest;

class AxCore_TestProgram : public ax::UnitTestProgram {
public:
	virtual void onRun() override {
		constexpr bool test_all = true;
		if constexpr (test_all) { 
			AX_TEST_GROUP(AxCore_Test)
		} else {
			// AX_TEST_GROUP(Test_Rtti);
			// AX_TEST_GROUP(Test_String);
			// AX_TEST_GROUP(Test_LinkedList);
			// AX_TEST_GROUP(Test_Array);
			AX_TEST_GROUP(Test_String);
		}
	}
};

int main() {
	AxCore_TestProgram test;
	return test.run();
}
