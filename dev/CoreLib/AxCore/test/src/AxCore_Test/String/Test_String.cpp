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
		
		String   str  (  "Testing");
		String8  str8 (u8"Testing");
		String16 str16( u"Testing");
		String32 str32( U"Testing");
		StringW  strW ( L"Testing");

		Int size_str   = sizeof(str  );
		Int size_str8  = sizeof(str8 );
		Int size_str16 = sizeof(str16);
		Int size_str32 = sizeof(str32);
		Int size_strW  = sizeof(strW );

		Int total_size = size_str + size_str8 + size_str16 + size_str32 + size_strW;
		AX_UNUSED(total_size);
	}
};

} // namespace

void Test_String() {
	AX_TEST_RUN_CASE(ax::Test_String::test_case1)
}

