import AxCore_Test.PCH;

namespace ax {

class Test_Math : public UnitTestClass {
public:
	void test_pow() {
		AX_TEST_EQ(Math::nextPow2(10), 16);
	}

	void test_align() {
		auto scope = s_testRequest().scopedVerbose();
		AX_TEST_EQ(Math::alignUp( 10, 4),  12);
		AX_TEST_EQ(Math::alignUp(-10, 4), -12);
		AX_TEST_EQ(Math::alignDown( 10, 4),  8);
		AX_TEST_EQ(Math::alignDown(-10, 4), -8);

		AX_TEST_EQ(Math::alignUp( 2, 4),  4);
		AX_TEST_EQ(Math::alignUp(-2, 4), -4);
		AX_TEST_EQ(Math::alignDown( 2, 4), 0);
		AX_TEST_EQ(Math::alignDown(-2, 4), 0);
	}
	
};

} // namespace

void Test_Math() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Math::test_pow);
	AX_TEST_RUN_CASE(Test_Math::test_align);
}
