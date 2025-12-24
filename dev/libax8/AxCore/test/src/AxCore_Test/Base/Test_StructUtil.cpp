import AxCore_Test.PCH;

import AxCore.StructUtil;

namespace ax {

class Test_StructUtil : public UnitTestClass {
public:

	struct Data {
		Int    i;
		float  f;
	};

	void test_case1() {
//		Data data { .i = 1, .f = 2};
//		auto t = StructUtil<Data>::toTuple(std::move(data));
	
	}
	
};

} // namespace

void Test_StructUtil() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_StructUtil::test_case1);
}
