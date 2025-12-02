#include "AxUnitTest.h"

import AxCore.MetaType;
import AxCore.Rtti;

namespace ax {

class Test_Rtti : public UnitTestClass {
public:
	class Foo {
		AX_TYPE_INFO(Foo, NoBaseClass)		
	public:
		int x, y, z;
		struct MetaType : public AX_META_TYPE(Foo, NoBaseClass) {
			struct x : public AX_META_FIELD(This, x) {
			};
			
		};
	};

	class MyObject : public RttiObject {
		AX_RTTI_CLASS(MyObject, RttiObject)
	};
	
	void test_case1() {
		using TI = MetaTypeOf<Foo>;
		// AX_TEST_EQ(TI::s_name(),     "Foo");
		// AX_TEST_EQ(TI::x::s_name(),  "x");
		//		AX_TEST_EQ(TI::x::s_offset(), 0);

		Rtti* ti = rttiOf<Foo>();
		AX_UNUSED(ti);

		AX_LOG("name = {}", ti->name());
	}
};

} // namespace

void Test_Rtti() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Rtti::test_case1)
}

