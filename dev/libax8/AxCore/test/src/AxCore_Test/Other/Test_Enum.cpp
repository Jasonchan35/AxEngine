import AxCore_Test.PCH;

namespace ax {

#define AX_Example_ENUM_LIST(E) \
	E(None,) \
	E(A , = 1 << 0) \
	E(B , = 1 << 1) \
	E(C , = 1 << 2) \
	E(D , = 1 << 3) \
	E(AC, = (A | C)) \
//----
AX_ENUM_CLASS(AX_Example_ENUM_LIST, Example, u8)

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


	void test_case1() {
		Example t = Example::AC;
		AX_TEST_EQ(ax_enum_str(t), "AC");

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
	AX_TEST_RUN_CASE(Test_Enum::test_case1)
}

