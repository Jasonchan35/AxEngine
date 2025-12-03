#if 1
#include "AxUnitTest.h"
#include "AxCore/Reflection/AxMetaType.h"
#include "AxCore/Reflection/AxRtti.h"
//----- Reflection

import AxCore.MetaType;
import AxCore.Rtti;

namespace ax {

class Test_Rtti : public UnitTestClass {
public:
	template<class T>
	class Foo {
		AX_TYPE_INFO(Foo, NoBaseClass)
	public:
		struct MetaType;
		int x, y, z;
	};

	template<class T, Int N>
	class Bar : public Foo<T> {
		AX_TYPE_INFO(Bar, Foo<T>)
	public:
		struct MetaType;
		int bar;
	}; 

	class MyObject : public RttiObject {
		AX_RTTI_CLASS(MyObject, RttiObject)
	};

	void test_case1() {
		using TestFoo = Foo<void>;
		using TestBar = Bar<StrView, 99>;
		
		using TI = MetaTypeOf_< TestBar >;
		// AX_TEST_EQ(TI::s_name(),     "Foo");
		// AX_TEST_EQ(TI::x::s_name(),  "x");
		//		AX_TEST_EQ(TI::x::s_offset(), 0);

		Rtti* ti = rttiOf< TestBar >();
		AX_UNUSED(ti);

		AX_LOG("name = {}", ti->name);
		for (auto& field : ti->fields) {
			AX_LOG("  field {}", field.name);
		}
	}
};

// struct Test_Rtti::MyObject::MetaType : public NoBaseClass::MetaType {};

template<class T>
struct Test_Rtti::Foo<T>::MetaType : AX_META_TYPE(Foo<T>) {
	struct x : AX_META_FIELD(x) {};
	struct y : AX_META_FIELD(y) {};

//	using OwnFields = Tuple<x, y>; 
};

template<class T, Int N>
struct Test_Rtti::Bar<T, N>::MetaType : AX_META_TYPE(AX_WRAP(Bar<T,N>)) {
//	using MetaThis = Bar<T, N>;
//	static NameId s_name() { return NameId("Bar"); }
};

} // namespace

void Test_Rtti() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Rtti::test_case1)
}
#else

void Test_Rtti() {}

#endif