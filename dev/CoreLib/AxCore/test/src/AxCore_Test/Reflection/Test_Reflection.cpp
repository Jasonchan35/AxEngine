#include "AxUnitTest.h"
#include "AxReflection.h"

namespace ax {

struct TestClass_Reflection : public UnitTestClass {
	class Foo {
	public:
		int x, y, z;

		// AX_TYPE_INFO(Foo, NoBaseClass) {
		// 	AX_FIELD_INFO(x) {};
		//
		// 	AX_FIELD_INFO(y) {
		// 		using Attrs = Tuple<Reflection::Attr_NonSerializable>;
		// 	};
		// 	AX_FIELD_INFO_WITH_NAME(z, "test_z") {
		// 	};
		//
		// 	using OwnFields = Tuple<x, y, z>;
		// };
	};

	class Foo2 : public Foo {
	public:
		String s;

		// AX_TYPE_INFO(Foo2, Foo) {
		// 	AX_FIELD_INFO(s) {};
		// 	using OwnFields = Tuple<s>;
		// };
	};

	class Foo3 : public Foo2 {
	public:
		// AX_TYPE_INFO(Foo3, Foo2) {
		// };
	};

	void test2(String s) {
	}

	struct DumpFieldTypes {
		template<Int index, class Field>
		static void onEach() {
			AX_TEST_PRINT("{} {}", index, Field::s_name());
		}
	};

	void test_case1(int xxx) {
		//		AX_TEST_ENABLE_VERBOSE_SCOPE()

		// using F = ax_typeof<Foo3>;
		// AX_TEST_EQ(F::s_name(),     "Foo3");
		// AX_TEST_EQ(F::x::s_name(),  "x");
		// AX_TEST_EQ(F::x::s_offset(), 0);
		//
		// AX_TEST_EQ(F::y::s_name(),  "y");
		// AX_TEST_EQ(F::y::s_offset(), sizeof(int));
		//
		// AX_TEST_EQ(F::z::s_name(),   "test_z");
		// AX_TEST_EQ(F::z::s_offset(), sizeof(int) * 2);
		//
		// F::AllFields::s_forEachType<DumpFieldTypes>();
		//
		// AX_TEST_EQ(ax_typeof<Foo2>::BaseInfo::s_name(), "Foo");
		// AX_TEST_EQ(ax_typeof<Foo3>::BaseInfo::s_name(), "Foo2");
		//
		// AX_TEST_EQ(ax_typeof<Foo >::AllFields::kSize, 3);
		// AX_TEST_EQ(ax_typeof<Foo2>::AllFields::kSize, 4);
		// AX_TEST_EQ(ax_typeof<Foo3>::AllFields::kSize, 4);
	}

	void test_Vec() {
		// using V = ax_typeof<Vec2i>;
		//
		// static StrView v2_name = V::s_name();
		// AX_UNUSED(v2_name);
		// AX_TEST_PRINT("{}", v2_name);
		// V::AllFields::s_forEachType<DumpFieldTypes>();
	}
};

} // namespace

void Test_Reflection() {
	AX_TEST_RUN_CASE(ax::TestClass_Reflection::test_case1, 11)
}

