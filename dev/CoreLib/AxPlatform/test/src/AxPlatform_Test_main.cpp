#include "AxUnitTest.h"

import AxPlatform.UnitTest;

namespace ax {
class AxPlatform_TestProgram : public ax::UnitTestProgram {
public:
	virtual void onRun() override {
		constexpr bool test_all = true;
		if constexpr(test_all) {
			// ALL
			AX_TEST_GROUP(AxPlatform_Test)
		} else {
			//	AX_TEST_GROUP(Test_Math);
		}
	}
};

} // namespace

int main() {
	ax::AxPlatform_TestProgram test;
	return test.run();
}
