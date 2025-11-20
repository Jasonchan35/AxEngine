#include "AxUnitTest.h"

namespace ax {

struct Test_String : public UnitTestCase {

	void run() {
		String s("Testing");
		s.append("123");
		
	}
};

} // namespace

void Test_String() {
	ax::Test_String().run();
}

