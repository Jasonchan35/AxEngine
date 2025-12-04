#include "AxCore_Test-pch.h"
import AxCore_Test._PCH;

import AxCore.DateTime;

namespace ax {

class Test_DateTime : public UnitTestClass {
public:
	void test_case1() {
		// auto verbose = s_testRequest().scopedVerbose();
		
		auto timeStr = StrView("2023-07-19 11:22:33");

		DateTime dt;
		dt.tryParse(timeStr);

		AX_TEST_EQ(dt.year(),    2023);
		AX_TEST_EQ(dt.month(),   7);
		AX_TEST_EQ(dt.day(),     19);
		AX_TEST_EQ(dt.weekday(), Weekday::Wednesday);
		AX_TEST_EQ(dt.hour(),    11);
		AX_TEST_EQ(dt.minute(),  22);
		AX_TEST_EQ(dt.second(),  33);
		
		auto str = Fmt("{}", dt);
		AX_TEST_EQ(str, timeStr);
	}
};

} // namespace

void Test_DateTime() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_DateTime::test_case1)
}

