#include "AxUnitTest.h"

import <string_view>;
import <format>;
import <iostream>;

namespace ax {

struct TestCase_Format : public UnitTestCase {
	struct CustmoData {

		template<class FMT_CH>
		constexpr static void onFormatParse(FormatParseContext_<FMT_CH> &  ctx) {
		
		}

		template<class FMT_CH>
		constexpr void onFormat(FormatContext_<FMT_CH> & ctx) const {
			ax_format_to(ctx, "{}", 100);
		}	
	};
	
	void run() {
		{
			auto str = ax_format("{} {}", 1, 2.3);
			Debug::_internal_log(str.c_str());
		}
		
		CustmoData data;
		{
			auto str = ax_format("{}", data);
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
	ax::TestCase_Format().run();
}

