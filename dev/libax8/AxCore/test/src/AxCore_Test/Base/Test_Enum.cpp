import AxCore_Test.PCH;

namespace ax {

class Test_Enum : public UnitTestClass {
public:
	void test_case1() {
		
	}
};

} // namespace

void Test_Enum() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Enum::test_case1)
}

