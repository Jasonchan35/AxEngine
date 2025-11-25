#include "AxUnitTest.h"

namespace ax {

class Test_String : public UnitTestClass {
public:

	void test_case1() {
		StrView   view  (  "Testing");
		StrView8  view8 (u8"Testing");
		StrView16 view16( u"Testing");
		StrView32 view32( U"Testing");
		StrViewW  viewW ( L"Testing");

		AX_UNUSED(view);
		AX_UNUSED(view8);
		AX_UNUSED(view16);
		AX_UNUSED(view32);
		AX_UNUSED(viewW);
		
		String   str  (  "Testing");
		String8  str8 (u8"Testing");
		String16 str16( u"Testing");
		String32 str32( U"Testing");
		StringW  strW ( L"Testing");
		
		AX_UNUSED(str);
		AX_UNUSED(str8);
		AX_UNUSED(str16);
		AX_UNUSED(str32);
		AX_UNUSED(strW);

		//constexpr
		// constexpr String constStr("constexpr string");
	}
};

} // namespace

void Test_String() {
	AX_TEST_RUN_CASE(ax::Test_String::test_case1)
}

