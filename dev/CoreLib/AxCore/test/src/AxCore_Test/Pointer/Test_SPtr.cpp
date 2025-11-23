#include "AxUnitTest.h"

import AxCore.WPtr;

namespace ax {

class Test_SPtr : public UnitTestClass {
public:
#if 0
	class TestForwardDeclareObject;
	void test_forward_declare() {
		SPtr<TestForwardDeclareObject> s;
	}
#endif

	class TestObjectBase : public WPtrReferenable {
	public:
		Int base_i = 1;
	};

	class TestObject : public TestObjectBase {
	public:
		TestObject(Int a_, Int b_) 
			: a(a_)
			, b(b_) 
		{}

		template<class CH>
		void onFmt(Format_<CH>& ctx) {
			ctx << "a" << a;
			ctx.newline();

			ctx << "b" << b;
			ctx.newline();
		}

		Int a = 899;
		Int b = 999;
	};


	void test_weakPtr() {
		WPtr<TestObject>	w;

		{
			auto obj1 = SPtr_new<TestObject>(AX_ALLOC_REQ, 1,2);
			w.ref(obj1);

			auto obj2 = SPtr_new<TestObject>(AX_ALLOC_REQ, 3,2);
			w.ref(obj2);

			auto r = w.getSPtr();
			r->a = 10;
		}

		{
			auto r = w.getSPtr();
			AX_TEST_EQ(r.ptr() == nullptr, true);
		}
	}

	void test_upcast() {
		auto obj1 = SPtr_new<TestObject>(AX_ALLOC_REQ, 4,2);

		SPtr<TestObjectBase> b(obj1);
		SPtr<TestObjectBase> c;
		c = obj1;
	}
};

} // namespace

void Test_SPtr() {
	AX_TEST_RUN_CASE(ax::Test_SPtr::test_weakPtr)
	AX_TEST_RUN_CASE(ax::Test_SPtr::test_upcast)
}

