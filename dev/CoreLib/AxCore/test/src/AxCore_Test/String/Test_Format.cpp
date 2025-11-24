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
			fmt.format("CustmoData={}", 100);
		}	
	};
	
	void test_case1() {
		{
			auto ret = Fmt("int=[{:6}] sz=[{:6}] wsz=[{:6}]", 1, "abc", L"wchar");
			Debug::_internal_log(ret.c_str());
		}
		{
			String   str = "str";
			IString& istr = str;
			StrView  sv = str;
			auto ret = Fmt("sv=[{:6}] istr=[{:6}] str=[{:6}]", sv, istr, str);
			Debug::_internal_log(ret.c_str());
		}
		{
			AX_TEST_EQ(Fmt("[{:4}][{:4.1}]", 1, 0.123), "[   1][ 0.1]");
			// auto str = Fmt("{} {}", 1, 2.3);
			// Debug::_internal_log(str.c_str());
		}
		
		{
			CustmoData data;
			auto str = Fmt("{}", data);
			Debug::_internal_log(str.c_str());
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

} // namespace

void Test_Format() {
	AX_TEST_RUN_CASE(ax::Test_Format::test_case1)
}
