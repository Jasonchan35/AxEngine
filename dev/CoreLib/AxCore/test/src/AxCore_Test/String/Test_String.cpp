#include "AxUnitTest.h"

namespace ax {

struct Test_String : public UnitTestCase {

	void run() {
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

		Array<Int> test;
		test.append(1);
		test.append(2);
		test.append(3);
	}
};

} // namespace

void Test_String() {
	ax::Test_String().run();
}

