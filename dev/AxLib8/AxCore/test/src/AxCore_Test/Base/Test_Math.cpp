import AxCore_Test.PCH;
import AxCore.Float16;

namespace ax {

class Test_Math : public UnitTestClass {
public:
	void test_pow() {
		AX_TEST_EQ(Math::nextPow2(10), 16);
	}

	void test_align() {
		auto scope = s_testRequest().scopedVerbose();
		AX_TEST_EQ(Math::alignTo( 10, 4),  12);
		AX_TEST_EQ(Math::alignTo(-10, 4), -12);
		AX_TEST_EQ(Math::alignDown( 10, 4),  8);
		AX_TEST_EQ(Math::alignDown(-10, 4), -8);

		AX_TEST_EQ(Math::alignTo( 2, 4),  4);
		AX_TEST_EQ(Math::alignTo(-2, 4), -4);
		AX_TEST_EQ(Math::alignDown( 2, 4), 0);
		AX_TEST_EQ(Math::alignDown(-2, 4), 0);
	}
	
	void test_Float16() {
		auto zero = f16::s_zero();
		auto one  = f16::s_one();
		auto half = f16::s_half();
		auto two  = f16::s_two();
		
		AX_TEST_ALMOST_EQ(zero.to_f32(), float(0));
		AX_TEST_ALMOST_EQ(one.to_f32(),  float(1));
		AX_TEST_ALMOST_EQ(half.to_f32(), float(0.5));
		AX_TEST_ALMOST_EQ(two.to_f32(), float(2));
	}
	
};

} // namespace

void Test_Math() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Math::test_pow);
	AX_TEST_RUN_CASE(Test_Math::test_align);
	AX_TEST_RUN_CASE(Test_Math::test_Float16);
}
