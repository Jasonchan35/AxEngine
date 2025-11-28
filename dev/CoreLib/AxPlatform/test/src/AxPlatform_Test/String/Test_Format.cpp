#include "AxUnitTest.h"

namespace ax {

class Test_Format : public UnitTestClass {
public:
	struct CustmoData {
		template<class FMT_CH>
		constexpr static void onFormatParse(Format_<FMT_CH> & fmt) {
			
		}

		template<class FMT_CH>
		constexpr void onFormat(Format_<FMT_CH> & fmt) const {
			fmt << "abcd";
			fmt << Fmt("CustmoData={}", 100);
		}	
	};

	void test_format_custom_type() {
		CustmoData data;
		auto str = Fmt("{}", data);
		__ax_internal_log(str.c_str());

		FmtTo(str, L" {}", StrView("strA"));
		//			FmtTo(str,  " {}", u"str16");
		__ax_internal_log(str.c_str());
	}

	template<class T>
	void test_utf() {
		String_<T> str;
		FmtTo(str, AX_STR_A( " fmtA"));
		FmtTo(str, AX_STR_W( " fmtW"));
		FmtTo(str, AX_STR_8( " fmt8"));
		FmtTo(str, AX_STR_16(" fmt16"));
		FmtTo(str, AX_STR_32(" fmt32"));

		FmtTo(str, " {}", AX_STR_A( "argA"));
		FmtTo(str, " {}", AX_STR_W( "argW"));
		FmtTo(str, " {}", AX_STR_8( "arg8"));
		FmtTo(str, " {}", AX_STR_16("arg16"));
		FmtTo(str, " {}", AX_STR_32("arg32"));

		auto msg = TempStringA::s_utf(str);
		__ax_internal_log(msg.c_str());
	}	
	
	void test_case1() {
		{
			auto ret = Fmt("int=[{:6}] sz=[{:6}] wsz=[{:6}]", 1, "abc", L"wchar");
			__ax_internal_log(ret.c_str());
//			AX_LOG("test {}", 123);
		}
		{
			String   str = "str";
			IString& istr = str;
			StrView  sv = str;
			auto ret = Fmt("sv=[{:6}] istr=[{:6}] str=[{:6}]", sv, istr, str);
			__ax_internal_log(ret.c_str());
		}
		{
			AX_TEST_EQ(Fmt("[{:4}][{:4.1}]", 1, 0.123), "[   1][ 0.1]");
			// auto str = Fmt("{} {}", 1, 2.3);
			//__ax_internal_log(str.c_str());
		}
		

// 		
// 		String format_string("format test {}");
// 		constexpr StrView format_sv("format test view {}");
//
// //		auto compile_fmt_check = ax_format("format test {} {} {}", 1, 2); // compile time format string check
//
// 		{ auto str = StringA_N<2>::s_format("format test {} {}", 1, 2); }
// 		{ auto str = StringW_N<2>::s_format(L"format test {} {}", 1, 2); }
//
// 		{ auto str = StringA_N<2>::s_format(format_string, 1, 2); }
// 		{ auto str = ax_format("format test {} {}", 1, 2); }
// 		{ auto str = ax_format( L"format test {} {} {}", 1, 2, "sz"); }
// 		{ auto str = ax_format(format_sv, 1); }
// 		{ auto str = ax_format(format_string, 1); }
		
#if AX_LANG_CPP_23 
		auto buf8  = ax_format(u8"format test {} {} ", 1, sv);
		auto buf16 = ax_format( u"format test {} {} ", 1, sv);
		auto buf32 = ax_format( U"format test {} {} ", 1, sv);
#endif
	}
};

void Test_Format_Func() {
	AX_TEST_RUN_CASE(Test_Format::test_case1)
	AX_TEST_RUN_CASE(Test_Format::test_format_custom_type)
	AX_TEST_RUN_CASE(Test_Format::test_utf<CharA>)
	AX_TEST_RUN_CASE(Test_Format::test_utf<CharW>)
	AX_TEST_RUN_CASE(Test_Format::test_utf<Char8>)
	AX_TEST_RUN_CASE(Test_Format::test_utf<Char16>)
	AX_TEST_RUN_CASE(Test_Format::test_utf<Char32>)
}

} // namespace

void Test_Format() { ax::Test_Format_Func(); }
