#include "AxCore_Test-pch.h"
import AxCore_Test.PCH;

import AxCore.Dict;

namespace ax {

class Test_Dict : public UnitTestClass {
public:

	struct TestData : public NonCopyable {
		TestData(Int a_, Int b_) : a(a_), b(b_) {
//			AX_LOG("TestData {} {}", a, b);
		}
		TestData(TestData &&) = default;
		
		~TestData() {
//			AX_LOG("~TestData {} {}", a, b);
		}
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
		
		dict.clear();
	}
};

} // namespace

void Test_Dict() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Dict::test_case1)
}

