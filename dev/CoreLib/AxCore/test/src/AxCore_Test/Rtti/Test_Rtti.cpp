#if 1
#include "AxUnitTest.h"
//----- Reflection

#define AX_TYPE_INFO(T, BASE) \
private: \
	using This = T; \
	using Base = BASE; \
private: \
//------

#define AX_RTTI_CLASS(T, BASE) \
	AX_TYPE_INFO(T, BASE) \
public: \
	static  Rtti* s_rtti ()					{ return rttiOf<T>(); } \
	virtual Rtti* getRtti() const override	{ return rttiOf<T>(); } \
private: \
//-----------

#define AX_META_TYPE(T, BASE) MetaType_<T, BASE, []()->StrView{ return #T; } >
#define AX_META_FIELD(V) MetaField_<MetaThis, &MetaThis::V, []()->StrView{ return #V; } >

import AxCore.MetaType;
import AxCore.Rtti;

namespace ax {

class Test_Rtti : public UnitTestClass {
public:

	template<class T>
	class Foo {
		AX_TYPE_INFO(Foo, NoBaseClass)
	public:
		int x, y, z;

		struct MetaType;
	};

	class MyObject : public RttiObject {
		AX_RTTI_CLASS(MyObject, RttiObject)
	};
	
	void test_case1() {
		using TestFoo = Foo<void>;
		
		using TI = MetaTypeOf< TestFoo >;
		// AX_TEST_EQ(TI::s_name(),     "Foo");
		// AX_TEST_EQ(TI::x::s_name(),  "x");
		//		AX_TEST_EQ(TI::x::s_offset(), 0);

		Rtti* ti = rttiOf< TestFoo >();
		AX_UNUSED(ti);

		AX_LOG("name = {}", ti->name);
	}
};

template<class T>
struct Test_Rtti::Foo<T>::MetaType : public MetaTypeOf_<NoBaseClass> {
	// : public MetaType_<Foo<T>, []()->StrView{ return "Foo"; } > {
//: public AX_META_TYPE(Foo<T>, NoBaseClass) {
	using MetaThis = Foo<T>;
	static NameId s_name() { return NameId("Foo"); }
//	static StrView s_name() { return "Foo"; }

	struct x : public AX_META_FIELD(x) {};
	struct y : public AX_META_FIELD(y) {};

	using OwnFields = Tuple<x, y>; 
};


} // namespace

void Test_Rtti() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Rtti::test_case1)
}
#else

void Test_Rtti() {}

#endif