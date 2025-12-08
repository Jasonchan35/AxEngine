import AxCore_Test.PCH;

namespace ax {

class Test_Math : public UnitTestClass {
public:
	void test_case1() {
		AX_TEST_EQ(Math::nextPow2(10), 16);
	}
	
};

} // namespace

void Test_Math() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Math::test_case1);
}
