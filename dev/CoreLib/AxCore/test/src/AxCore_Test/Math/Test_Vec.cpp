#include "AxUnitTest.h"

import AxCore.Vec;

namespace ax {

class Test_Vec : public UnitTestClass {
public:

	void test_case1() {
		AX_TEST_EQ(Math::epsilon<Vec4f>.y, f32_epsilon);
		
		// AX_TEST_EQ(sizeof(Vec3f_NoSIMD), 4 * 3);
		// AX_TEST_EQ(sizeof(Vec3f_SSE)   , 4 * 4);
		AX_TEST_EQ(sizeof(Vec4f_NoSIMD), 4 * 4);
		AX_TEST_EQ(sizeof(Vec4f_SSE)   , 4 * 4);

		{
			// Vec3f a(1, 2, 3);
			// Vec3f b(2, 3, 4);
			// auto c = a + b;
			// AX_UNUSED(c);
		}

		{ // test constexpr
			constexpr Vec4f a(1, 2, 3, 4);
			constexpr Vec4f b(2, 3, 4, 5);
			constexpr auto c = a + b;
			AX_TEST_ALMOST_EQ(c, Vec4f(3,5,7,9));
		}

		{
			Vec4f a(1, 2, 3, 4);
			Vec4f b(2, 3, 4, 5);
			auto c = a + b;
			
			AX_TEST_ALMOST_EQ(c, Vec4f(3,5,7,9));
			AX_TEST_NOT_ALMOST_EQ(c, Vec4f(3,9,7,9));
			AX_TEST_NOT_ALMOST_EQ(c, Vec4f(3,1,7,9));
		}
	}
	
	template<class T, CpuSIMD SIMD>
	struct SIMD_Test {
		using Vec = Vec4_<T, SIMD>; 
		Vec add;  
		Vec sub;
		Vec mul;
		Vec div;

		SIMD_Test() {
			auto a = Vec(1, 2, 3, 4);
			auto b = Vec(2, 3, 4, 5);
			add = a + b;
			sub = a - b;
			mul = a * b;
			div = a / b;
		}
	};

	
	
	template<class T>
	void test_SSE() {
		SIMD_Test<T, CpuSIMD::NoSIMD>	noSIMD;
		SIMD_Test<T, CpuSIMD::SSE>		sse;
		AX_TEST_ALMOST_EQ(sse.add, noSIMD.add);
		AX_TEST_ALMOST_EQ(sse.sub, noSIMD.sub);
		AX_TEST_ALMOST_EQ(sse.mul, noSIMD.mul);
		AX_TEST_ALMOST_EQ(sse.div, noSIMD.div);
	}
};

void Test_Vec_Func() {
	AX_TEST_RUN_CASE(Test_Vec::test_case1)
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<f32>)
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<f64>)
}


} // namespace

void Test_Vec() { ax::Test_Vec_Func(); }
