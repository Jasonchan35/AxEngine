#include "AxUnitTest.h"

import <string_view>;
import <format>;

namespace ax {

struct TestCase_Format : public UnitTestCase {
	void run() {
		StrView format = "format test view {} {}";
	
//		auto compile_fmt_check = ax_format("format test {} {} {}", 1, 2); // compile time format string check

		auto sf1  = StringA_N<2>::s_format("format test {} {}", 1, 2);
		auto sf1v = StringW_N<2>::s_format(L"format test {} {}", 1, 2);
		
		auto buf   = ax_format("format test {} {}", 1, 2);
		// Debug::_internal_log(buf.c_str());
		
		auto bufW  = ax_format( L"format test {} {} {}", 1, 2, "sz");
		
#if AX_LANG_CPP_23 
		auto buf8  = ax_format(u8"format test {} {} ", 1, sv);
		auto buf16 = ax_format( u"format test {} {} ", 1, sv);
		auto buf32 = ax_format( U"format test {} {} ", 1, sv);
#endif

		// StrView format_string = "format test {} {}";
		// auto sv_buf = ax_format_(format_string, 1, sv);
		
	}
};

} // namespace

void Test_Format() {
	ax::TestCase_Format().run();
}

