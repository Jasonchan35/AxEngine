#include "AxUnitTest.h"

import AxCore.NameId;

namespace ax {

class Test_String : public UnitTestClass {
public:

	void test_case1() {
		StrView   view  (  "Testing");
		StrView8  view8 (u8"Testing");
		StrView16 view16( u"Testing");
		StrView32 view32( U"Testing");
		StrViewW  viewW ( L"Testing");

		AX_UNUSED(view);
		AX_UNUSED(view8);
		AX_UNUSED(view16);
		AX_UNUSED(view32);
		AX_UNUSED(viewW);
		
		String   str  (  "Testing");
		String8  str8 (u8"Testing");
		String16 str16( u"Testing");
		String32 str32( U"Testing");
		StringW  strW ( L"Testing");
		
		AX_UNUSED(str);
		AX_UNUSED(str8);
		AX_UNUSED(str16);
		AX_UNUSED(str32);
		AX_UNUSED(strW);
	}

	void test_PersistString() {
		auto a = PersistString::s_make("Testing");
		auto b = PersistString::s_make("Testing");

		AX_TEST_EQ(a, b);
		AX_TEST_EQ(a.view(), StrView("Testing"));
	}

	/*
	void test_case2_string_construction() {
		String str1("Hello");
		AX_TEST_EQ(str1.size(), 5);
		AX_TEST_EQ(str1, "Hello");

		String str2(str1);
		AX_TEST_EQ(str2.size(), 5);
		AX_TEST_EQ(str2, "Hello");

		String str3("");
		AX_TEST_EQ(str3.size(), 0);
	}
	*/
	void test_case3_string_append() {
		String str("Hello");
		str.append(" World");
		AX_TEST_EQ(str, "Hello World");
		AX_TEST_EQ(str.size(), 11);

		str.append('!');
		AX_TEST_EQ(str, "Hello World!");
		AX_TEST_EQ(str.size(), 12);
	}

	void test_case4_string_comparison() {
		String str1("test");
		String str2("test");
		String str3("TEST");

		AX_TEST_EQ(str1, str2);
		AX_TEST_NOT_EQ(str1, str3);

		AX_TEST_CHECK(str1.equals("test"));
		AX_TEST_CHECK(!str1.equals("TEST"));
	}

	void test_case5_string_startswith_endswith() {
		String str("Hello World");

		AX_TEST_CHECK(str.startsWith("Hello"));
		AX_TEST_CHECK(str.endsWith("World"));
		AX_TEST_CHECK(!str.startsWith("World"));
		AX_TEST_CHECK(!str.endsWith("Hello"));
	}

	void test_case6_string_clear() {
		String str("Hello");
		AX_TEST_EQ(str.size(), 5);

		str.clear();
		AX_TEST_EQ(str.size(), 0);
		AX_TEST_CHECK(!str);
	}

	void test_case7_string_reserve() {
		String str;
		Int oldCapacity = str.capacity();

		str.reserve(100);
		Int newCapacity = str.capacity();
		AX_TEST_GTEQ(newCapacity, 100);
		AX_TEST_GTEQ(newCapacity, oldCapacity);
	}

	void test_case8_string_access() {
		String str("Hello");

		AX_TEST_EQ(str[0], 'H');
		AX_TEST_EQ(str[4], 'o');
		AX_TEST_EQ(str.back(), 'o');
		AX_TEST_EQ(str.back(1), 'l');
	}

	void test_case9_string_slice() {
		String str("Hello World");

		StrView slice1 = str.slice(0, 5);
		AX_TEST_EQ(slice1, "Hello");

		StrView slice2 = str.slice(6, 5);
		AX_TEST_EQ(slice2, "World");

		StrView slice3 = str.sliceFrom(6);
		AX_TEST_EQ(slice3, "World");

		StrView slice4 = str.sliceBack(5);
		AX_TEST_EQ(slice4, "World");
	}

	void test_case10_string_c_str() {
		String str("Test");

		const Char* cStr = str.c_str();
		AX_TEST_CHECK(cStr != nullptr);
		AX_TEST_EQ(cStr[0], 'T');
		AX_TEST_EQ(cStr[4], '\0');
	}

	/*
	void test_case11_string_move() {
		String str1("Hello");
		String str2;

		str2 = std::move(str1);
		AX_TEST_EQ(str2, "Hello");
		AX_TEST_EQ(str2.size(), 5);
	}
	*/

	void test_case12_string_comparison_operators() {
		String str1("abc");
		String str2("def");

		AX_TEST_LT(  str1, str2);
		AX_TEST_GT(  str2, str1);
		AX_TEST_LTEQ(str1, str1);
		AX_TEST_GTEQ(str1, str1);
	}

	void test_split() {
		String str1("apple and orange and banana");
		String str2("is");

		AX_TEST_EQ(str1.split("and"),							Pair_make(StrView("apple "), StrView(" orange and banana")));
		AX_TEST_EQ(str1.split("And", StrCase::Ignore),			Pair_make(StrView("apple "), StrView(" orange and banana")));
	
		AX_TEST_EQ(str1.splitBack("and"),						Pair_make(StrView("apple and orange "), StrView(" banana")));
		AX_TEST_EQ(str1.splitBack("And", StrCase::Ignore),		Pair_make(StrView("apple and orange "), StrView(" banana")));

		AX_TEST_EQ(str1.splitByChar('n'),						Pair_make(StrView("apple a"), StrView("d orange and banana")));
		AX_TEST_EQ(str1.splitByChar('N', StrCase::Ignore),		Pair_make(StrView("apple a"), StrView("d orange and banana")));

		AX_TEST_EQ(str1.splitByCharBack('n'),					Pair_make(StrView("apple and orange and bana"), StrView("a")));
		AX_TEST_EQ(str1.splitByCharBack('N', StrCase::Ignore),	Pair_make(StrView("apple and orange and bana"), StrView("a")));
	}
	
	void test_NameId() {
		auto a = AX_NAMEID("Test123");
		AX_TEST_EQ(a.name(), "Test");
		AX_TEST_EQ(a.id(),   123);
	}
};

} // namespace

void Test_String() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_String::test_case1)
	AX_TEST_RUN_CASE(Test_String::test_PersistString)

	//AX_TEST_RUN_CASE(Test_String::test_case2_string_construction)
	AX_TEST_RUN_CASE(Test_String::test_case3_string_append)
	AX_TEST_RUN_CASE(Test_String::test_case4_string_comparison)
	AX_TEST_RUN_CASE(Test_String::test_case5_string_startswith_endswith)
	AX_TEST_RUN_CASE(Test_String::test_case6_string_clear)
	AX_TEST_RUN_CASE(Test_String::test_case7_string_reserve)
	AX_TEST_RUN_CASE(Test_String::test_case8_string_access)
	AX_TEST_RUN_CASE(Test_String::test_case9_string_slice)
	AX_TEST_RUN_CASE(Test_String::test_case10_string_c_str)
	//AX_TEST_RUN_CASE(Test_String::test_case11_string_move)
	AX_TEST_RUN_CASE(Test_String::test_case12_string_comparison_operators)
	AX_TEST_RUN_CASE(Test_String::test_split)
	AX_TEST_RUN_CASE(Test_String::test_NameId)
}

