import AxCore_Test.PCH;

namespace ax {

class Test_BasicType : public UnitTestClass {
	using This = Test_BasicType;
public:
	void test_case1() {
		
	}
};

} // namespace

void Test_BasicType() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_BasicType::test_case1)
}

