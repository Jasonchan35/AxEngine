#include "AxUnitTest.h"

import AxCore.Dict;

namespace ax {

class Test_Dict : public UnitTestClass {
public:

	struct TestData : public NonCopyable {
		TestData(Int a_, Int b_) : a(a_), b(b_) {}
		Int a;
		Int b;
	};

	
	void test_case1() {
		Dict<String, TestData> dict;

		for (Int i = 0; i < 10; i++) {
			dict.add(Fmt("foo{}", i), 10, i);
		}

		auto* p = dict.find("foo2");
		AX_UNUSED(p);
	}
};

} // namespace

void Test_Dict() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Dict::test_case1)
}

