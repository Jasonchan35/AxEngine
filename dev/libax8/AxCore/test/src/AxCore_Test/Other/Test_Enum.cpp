import AxCore_Test.PCH;

namespace TestNamespace {
	using namespace ax;

	#define AX_TestNamespace_ENUM_LIST(E) \
		E(A,) \
	//----
	AX_ENUM_CLASS_NO_STR(AX_TestNamespace_ENUM_LIST, FooEnum, u32)
} namespace ax {
	AX_ENUM_STR_UTIL(AX_TestNamespace_ENUM_LIST, TestNamespace::FooEnum)
}

namespace ax {

#define AX_Example_ENUM_LIST(E) \
	E(None,) \
	E(A,   ) \
	E(B,   ) \
	E(C,   ) \
	E(D,=10) \
	E(_COUNT,) \
//----
AX_ENUM_CLASS(AX_Example_ENUM_LIST, Example, u8)

#define AX_ExampleFlags_ENUM_LIST(E) \
	E(None,) \
	E(A , = 1 << 0) \
	E(B , = 1 << 1) \
	E(C , = 1 << 2) \
	E(D , = 1 << 3) \
//----
AX_ENUM_FLAGS_CLASS(AX_ExampleFlags_ENUM_LIST, ExampleFlags, u8)

class Test_Enum : public UnitTestClass {
public:

	struct Data {
	#if 0 // test vc warning: #warning C5262: C5038 (level 4)	data member 'member1' will be initialized after data member 'member2'
		Data(int aa, int bb) : b(bb), a(aa) {}
	#else
		Data(int aa, int bb) : a(aa), b(bb) {}
	#endif

		int a;
		int b;
	};

	void test_EnumNamespace() {
		TestNamespace::FooEnum t = TestNamespace::FooEnum::A;
		auto str = Fmt("{}", t);
		AX_TEST_EQ(str, "A");
	}
	
	void test_EnumFlags() {
		ExampleFlags t = ExampleFlags::A | ExampleFlags::C;
		auto str = Fmt("{}", t);
		AX_TEST_EQ(str, "A | C");

		ExampleFlags parseResult = ExampleFlags::None;
		StrView("A |B | C | D").tryParse(parseResult);
		AX_TEST_EQ(parseResult, ExampleFlags::A | ExampleFlags::B | ExampleFlags::C | ExampleFlags::D);
	}

	void test_Enum() {
		Example t = Example::C;
		AX_TEST_EQ(ax_enum_str(t), "C");

		Int sum_i = 0;
		for (auto e : Range_(ax_enum_int(Example::_COUNT))) {
			sum_i += (e);
		}
		AX_TEST_EQ(sum_i, 55);
		
		Int sum = 0;
		for (auto e : Range_(Example::_COUNT)) {
			sum += ax_enum_int(e);
		}

		AX_TEST_EQ(sum, 55);
		AX_TEST_EQ(sum, sum_i);

		switch (t) {
#if 0 // test vc warning: #warning C5262: switch-case fallthrough warning
			case Example::A: AX_LOG("A"); // AX_FALLTHROUGH
			case Example::B: AX_LOG("B"); AX_FALLTHROUGH
		#endif
			case Example::C: AX_LOG("C"); break;
			default: break;
		}
	}	
};

} // namespace

void Test_Enum() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Enum::test_Enum)
	AX_TEST_RUN_CASE(Test_Enum::test_EnumNamespace)
	AX_TEST_RUN_CASE(Test_Enum::test_EnumFlags)
}

