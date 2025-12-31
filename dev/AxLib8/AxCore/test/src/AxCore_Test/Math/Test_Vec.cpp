
import AxCore_Test.PCH;

import AxCore.Vec;
import AxCore.VecSimd;
import AxCore.Random;
import AxCore.Color;

namespace ax {

class Test_Vec : public UnitTestClass {
public:

	template<class T>
	void test_VecSimd() {
		using Vec = VecSimd_Data_<4, T, VecSimd::None>;
		T x = Random::getRange<T>(1, 100);
		constexpr Int N = 10;
		Vec a[N]; 
		Vec b[N];
		Vec c[N];
		for (Int i = 0; i < N; i++) {
			a[i] = Vec(1,2,3,x);
			b[i] = Vec(4,x,6,x);
		}
		for (Int i = 0; i < N; i++) {
			c[i] = a[i] + b[i];
		}
		AX_TEST_ALMOST_EQ(c[5].e[0], T(5));
		AX_TEST_ALMOST_EQ(c[5].e[2], T(9));
	}
	
	void test_case1() {
		AX_TEST_EQ(Math::epsilon_<Vec4f>().y, Math::epsilon_<f32>());
		// AX_TEST_EQ(sizeof(Vec3f_NoSIMD), 4 * 3);
		// AX_TEST_EQ(sizeof(Vec3f_SSE)   , 4 * 4);
		AX_TEST_EQ(sizeof(Vec4f_Basic), 4 * 4);
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

			auto cc = Vec4d::s_cast(c);
			AX_TEST_ALMOST_EQ(cc, Vec4d(3,5,7,9));
		}
	}
	
	template<class T, VecSimd SIMD>
	struct SIMD_Test {
		using Vec1 = Vec1_<T, SIMD>; 
		using Vec2 = Vec2_<T, SIMD>; 
		using Vec3 = Vec3_<T, SIMD>; 
		using Vec4 = Vec4_<T, SIMD>;
		
		Vec1 add1;  Vec1 sub1;	Vec1 mul1;	Vec1 div1;
		Vec2 add2;  Vec2 sub2;	Vec2 mul2;	Vec2 div2;
		Vec3 add3;  Vec3 sub3;	Vec3 mul3;	Vec3 div3;
		Vec4 add4;  Vec4 sub4;	Vec4 mul4;	Vec4 div4;

		Vec1 scalar_add1;  Vec1 scalar_sub1;	Vec1 scalar_mul1;	Vec1 scalar_div1;
		Vec2 scalar_add2;  Vec2 scalar_sub2;	Vec2 scalar_mul2;	Vec2 scalar_div2;
		Vec3 scalar_add3;  Vec3 scalar_sub3;	Vec3 scalar_mul3;	Vec3 scalar_div3;
		Vec4 scalar_add4;  Vec4 scalar_sub4;	Vec4 scalar_mul4;	Vec4 scalar_div4;

		Vec1 add_scalar1;  Vec1 sub_scalar1;	Vec1 mul_scalar1;	Vec1 div_scalar1;
		Vec2 add_scalar2;  Vec2 sub_scalar2;	Vec2 mul_scalar2;	Vec2 div_scalar2;
		Vec3 add_scalar3;  Vec3 sub_scalar3;	Vec3 mul_scalar3;	Vec3 div_scalar3;
		Vec4 add_scalar4;  Vec4 sub_scalar4;	Vec4 mul_scalar4;	Vec4 div_scalar4;
		
		SIMD_Test() {
			{
				auto a = Vec1(2);
				auto b = Vec1(1);
				add1 = a + b;	add_scalar1 = a + 1;	scalar_add1 = T(2) + b;
				sub1 = a - b;	sub_scalar1 = a - 1;	scalar_sub1 = T(2) - b;
				mul1 = a * b;	mul_scalar1 = a * 1;	scalar_mul1 = T(2) * b;
				div1 = a / b;	div_scalar1 = a / 1;	scalar_div1 = T(2) / b;
			}
			{
				auto a = Vec2(2, 3);
				auto b = Vec2(1, 2);
				add2 = a + b;	add_scalar2 = a + 1;	scalar_add2 = T(2) + b;
				sub2 = a - b;	sub_scalar2 = a - 1;	scalar_sub2 = T(2) - b;
				mul2 = a * b;	mul_scalar2 = a * 1;	scalar_mul2 = T(2) * b;
				div2 = a / b;	div_scalar2 = a / 1;	scalar_div2 = T(2) / b;
			}
			{
				auto a = Vec3(2, 3, 4);
				auto b = Vec3(1, 2, 3);
				add3 = a + b;	add_scalar3 = a + 1;	scalar_add3 = T(2) + b;
				sub3 = a - b;	sub_scalar3 = a - 1;	scalar_sub3 = T(2) - b;
				mul3 = a * b;	mul_scalar3 = a * 1;	scalar_mul3 = T(2) * b;
				div3 = a / b;	div_scalar3 = a / 1;	scalar_div3 = T(2) / b;
			}
			{
				auto a = Vec4(2, 3, 4, 5);
				auto b = Vec4(1, 2, 3, 4);
				add4 = a + b;	add_scalar4 = a + 1;	scalar_add4 = T(2) + b;
				sub4 = a - b;	sub_scalar4 = a - 1;	scalar_sub4 = T(2) - b;
				mul4 = a * b;	mul_scalar4 = a * 1;	scalar_mul4 = T(2) * b;
				div4 = a / b;	div_scalar4 = a / 1;	scalar_div4 = T(2) / b;
			}
		}
	};
	
	template<class T>
	void test_SSE() {
		SIMD_Test<T, VecSimd::None>	basic;
		SIMD_Test<T, VecSimd::SSE>	sse;
		AX_TEST_ALMOST_EQ(basic.add1, sse.add1);
		AX_TEST_ALMOST_EQ(basic.add2, sse.add2);
		AX_TEST_ALMOST_EQ(basic.add3, sse.add3);
		AX_TEST_ALMOST_EQ(basic.add4, sse.add4);
		
		AX_TEST_ALMOST_EQ(basic.sub1, sse.sub1);
		AX_TEST_ALMOST_EQ(basic.sub2, sse.sub2);
		AX_TEST_ALMOST_EQ(basic.sub3, sse.sub3);
		AX_TEST_ALMOST_EQ(basic.sub4, sse.sub4);

		AX_TEST_ALMOST_EQ(basic.mul1, sse.mul1);
		AX_TEST_ALMOST_EQ(basic.mul2, sse.mul2);
		AX_TEST_ALMOST_EQ(basic.mul3, sse.mul3);
		AX_TEST_ALMOST_EQ(basic.mul4, sse.mul4);

		AX_TEST_ALMOST_EQ(basic.div1, sse.div1);
		AX_TEST_ALMOST_EQ(basic.div2, sse.div2);
		AX_TEST_ALMOST_EQ(basic.div3, sse.div3);
		AX_TEST_ALMOST_EQ(basic.div4, sse.div4);
	}
};

void Test_Vec_Func() {
	AX_TEST_RUN_CASE(Test_Vec::test_VecSimd<f32>)
	AX_TEST_RUN_CASE(Test_Vec::test_VecSimd<f64>)

	AX_TEST_RUN_CASE(Test_Vec::test_case1)
	
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<i8>)
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<i16>)
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<i32>)
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<i64>)
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<u8>)
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<u16>)
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<u32>)
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<u64>)
	
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<f32>)
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<f64>)
	
	AX_TEST_RUN_CASE(Test_Vec::test_SSE<f16>)
}

template class Vec_<4, f32, VecSimd::None>;
template class Vec_<4, f32, VecSimd::SSE>;

} // namespace

void Test_Vec() { ax::Test_Vec_Func(); }
