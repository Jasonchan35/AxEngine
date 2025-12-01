#include "AxUnitTest.h"

namespace ax {

int g_testScopeValue = 10;

class Test_BasicType : public UnitTestClass {
	using This = Test_BasicType;
public:
	void test_ScopeValue() {
		int v = 100;
		{
			auto scope = ScopeValue(&v);
			v = 200;
		}
		AX_TEST_EQ(v, 100);
	}

	static void _func() {
		g_testScopeValue = 12;
	}
	
	void test_ScopeFunc() {
		{
			auto scope = ScopeFunc0<_func>();
			g_testScopeValue = 13;
		}
		AX_TEST_EQ(g_testScopeValue, 12);
	}

	int _memberValue = 22;
	void _memberFunc(int t) {
		_memberValue = t + 1;
	}

	void test_ScopeObjFunc() {
		{
			auto scope = ScopeObjFunc1<This, int, &This::_memberFunc>(this, 88);
			_memberValue = 44;
		}
		AX_TEST_EQ(_memberValue, 89);
	}
};

} // namespace

void Test_BasicType() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_BasicType::test_ScopeValue)
	AX_TEST_RUN_CASE(Test_BasicType::test_ScopeFunc)
	AX_TEST_RUN_CASE(Test_BasicType::test_ScopeObjFunc)
}

