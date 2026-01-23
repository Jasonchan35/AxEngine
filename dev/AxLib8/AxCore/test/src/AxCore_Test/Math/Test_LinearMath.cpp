
import AxCore_Test.PCH;

import AxCore.LinearMath;
import AxCore.VecSimd;
import AxCore.Random;
import AxCore.Color;

namespace ax {

class Test_LinearMath : public UnitTestClass {
public:

	template<class T>
	void test_VecSimd() {
		using Vec = VecSimd_Data_<4, T, VecSimd::Basic>;
		T x = Random::fromRange<T>(1, 100);
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
		using Vec1  = Vec1_<T, SIMD>; 
		using Vec2  = Vec2_<T, SIMD>; 
		using Vec3  = Vec3_<T, SIMD>; 
		using Vec4  = Vec4_<T, SIMD>;
		
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

		T dot3;
		Vec4 neg;
		
		SIMD_Test() {
			{
				auto a = Vec1(T(2));
				auto b = Vec1(T(1));
				add1 = a + b;	add_scalar1 = a + T(1);	scalar_add1 = T(2) + b;
				sub1 = a - b;	sub_scalar1 = a - T(1);	scalar_sub1 = T(2) - b;
				mul1 = a * b;	mul_scalar1 = a * T(1);	scalar_mul1 = T(2) * b;
				div1 = a / b;	div_scalar1 = a / T(1);	scalar_div1 = T(2) / b;
			}
			{
				auto a = Vec2(T(2), T(3));
				auto b = Vec2(T(1), T(2));
				add2 = a + b;	add_scalar2 = a + T(1);	scalar_add2 = T(2) + b;
				sub2 = a - b;	sub_scalar2 = a - T(1);	scalar_sub2 = T(2) - b;
				mul2 = a * b;	mul_scalar2 = a * T(1);	scalar_mul2 = T(2) * b;
				div2 = a / b;	div_scalar2 = a / T(1);	scalar_div2 = T(2) / b;
			}
			{
				auto a = Vec3(T(2), T(3), T(4));
				auto b = Vec3(T(1), T(2), T(3));
				add3 = a + b;	add_scalar3 = a + T(1);	scalar_add3 = T(2) + b;
				sub3 = a - b;	sub_scalar3 = a - T(1);	scalar_sub3 = T(2) - b;
				mul3 = a * b;	mul_scalar3 = a * T(1);	scalar_mul3 = T(2) * b;
				div3 = a / b;	div_scalar3 = a / T(1);	scalar_div3 = T(2) / b;
			}
			{
				auto a = Vec4(T(2), T(3), T(4), T(5));
				auto b = Vec4(T(1), T(2), T(3), T(4));
				add4 = a + b;	add_scalar4 = a + T(1);	scalar_add4 = T(2) + b;
				sub4 = a - b;	sub_scalar4 = a - T(1);	scalar_sub4 = T(2) - b;
				mul4 = a * b;	mul_scalar4 = a * T(1);	scalar_mul4 = T(2) * b;
				div4 = a / b;	div_scalar4 = a / T(1);	scalar_div4 = T(2) / b;
			}
			
			{
				auto a = Vec3(T(2), T(3), T(4));
				auto b = Vec3(T(1), T(2), T(3));
				dot3 = a.dot(b);
			}
			
			{
				auto a = Vec4(T(1), T(2), T(3), T(4));
				neg = -a;
			}
		}
	};

	template<class T>
	void test_SSE() {
		SIMD_Test<T, VecSimd::Basic>	basic;
		SIMD_Test<T, VecSimd::SSE>		sse;
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
		
		AX_TEST_ALMOST_EQ(basic.dot3, sse.dot3);
		AX_TEST_ALMOST_EQ(basic.neg,  sse.neg);
	}
	
	
	template<class T, VecSimd SIMD>
	struct SIMD_TestMat {
		using Vec3  = Vec3_<T, SIMD>;
		using Vec4  = Vec4_<T, SIMD>;
		using Quat4 = Quat4_<T, SIMD>;
		using Mat4  = Mat4_<T, SIMD>;
		
		Mat4 mat_mul;
		Vec3 mat_mul_vector;
		Vec4 mat_mul_point;
		Mat4 mat_TRS;

		SIMD_TestMat() {
			{
				Mat4 a(	10, 11, 12, 13,
					   14, 15, 16, 17,
					   20, 21, 22, 23,
					   24, 25, 26, 27);

				Mat4 b(	30, 31, 32, 33,
						34, 35, 36, 37,
						40, 41, 42, 43,
						44, 45, 46, 47);

				mat_mul = a.mulMatrix(b);
			
				Vec3 vec(1,2,3);
				Vec4 pt(1,2,3,4);
				mat_mul_vector = a.mulVector(vec);
				mat_mul_point  = a.mulPoint(pt);
			}
			
		
			{
				Vec3  pos(1,2,3);
				Quat4 rot = Quat4::s_eulerDeg({10, 20, 30});
				Vec3  scale(4,5,6);
				mat_TRS = Mat4::s_TRS(pos, rot, scale);
				
				Vec3  resultPos;
				Quat4 resultRot;
				Vec3  resultScale;
				
				mat_TRS.getTRS(resultPos, resultRot, resultScale);

				AX_TEST_ALMOST_EQ_EPSILON(pos,   resultPos  , T(0.1));
				AX_TEST_ALMOST_EQ_EPSILON(rot,   resultRot  , T(0.1));
				AX_TEST_ALMOST_EQ_EPSILON(scale, resultScale, T(0.1));
			}			
			
		}
	};
	
	template<class T>
	void testMat_SSE() {
		SIMD_TestMat<T, VecSimd::Basic>	basic;
		SIMD_TestMat<T, VecSimd::SSE>	sse;
		
		AX_TEST_ALMOST_EQ(basic.mat_mul, sse.mat_mul);
		AX_TEST_ALMOST_EQ(basic.mat_mul_vector, sse.mat_mul_vector);
		AX_TEST_ALMOST_EQ(basic.mat_mul_point , sse.mat_mul_point);
		AX_TEST_ALMOST_EQ(basic.mat_TRS,        sse.mat_TRS);
	}
	
};

void Test_LinearMath_Func() {
	AX_TEST_RUN_CASE(Test_LinearMath::test_VecSimd<f32>)
	AX_TEST_RUN_CASE(Test_LinearMath::test_VecSimd<f64>)

	AX_TEST_RUN_CASE(Test_LinearMath::test_case1)
	
	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<i8>)
	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<i16>)
	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<i32>)
	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<i64>)
	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<u8>)
	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<u16>)
	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<u32>)
	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<u64>)

	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<f16>)
	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<f32>)
	AX_TEST_RUN_CASE(Test_LinearMath::test_SSE<f64>)
	
//	AX_TEST_RUN_CASE(Test_LinearMath::testMat_SSE<f16>)
	AX_TEST_RUN_CASE(Test_LinearMath::testMat_SSE<f32>)
	AX_TEST_RUN_CASE(Test_LinearMath::testMat_SSE<f64>)
}

template class Vec_<4, f32, VecSimd::Basic>;
template class Vec_<4, f32, VecSimd::SSE>;

} // namespace

void Test_LinearMath() { ax::Test_LinearMath_Func(); }
