import AxCore_Test.PCH;

namespace ax {

int g_testScopeValue = 10;

class Test_Scoped : public UnitTestClass {
	using This = Test_Scoped;
public:
	void test_ScopeValue() {
		int v = 100;
		{
			auto scope = ScopedValue(&v);
			v = 200;
		}
		AX_TEST_EQ(v, 100);
	}
	
	static void _func() {
		g_testScopeValue = 12;
	}
	
	void test_ScopedFunc() {
		{
			auto scope = ScopedStaticFunc0<_func>();
			g_testScopeValue = 13;
		}
		AX_TEST_EQ(g_testScopeValue, 12);
	}

	int _memberValue = 22;
	void _memberFunc(int t) {
		_memberValue = t + 1;
	}

	void test_ScopedMemFunc() {
		{
			auto scope = ScopedMemFunc1<This, int, &This::_memberFunc>(this, 88);
			_memberValue = 44;
		}
		AX_TEST_EQ(_memberValue, 89);
	}
};

} // namespace

void Test_Scoped() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Scoped::test_ScopeValue)
	AX_TEST_RUN_CASE(Test_Scoped::test_ScopedFunc)
	AX_TEST_RUN_CASE(Test_Scoped::test_ScopedMemFunc)
}

