export module AxCore.UnitTest;

#include "AxBase.h"
export import AxCore.String;
export import AxCore.Debug;

export namespace ax {

struct UnitTestRequest : public NonCopyable {
	// JsonValue	options;
};

class UnitTestProgram : public NonCopyable {
public:
	UnitTestProgram();

	UnitTestRequest testRequest;
	
	static UnitTestProgram* s_get();
	
	virtual      ~UnitTestProgram() = default;
	virtual void onRun() = 0;
	int          run();
	int          waitKeyPress();
};

struct UnitTestCase : public NonCopyable {
	static UnitTestRequest& s_testRequest() { return UnitTestProgram::s_get()->testRequest; }
};

inline
bool UnitTest_Validate(const SrcLoc& loc, bool success, const char* expr_str) {
	bool verbose = true;

	if (success && !verbose)
		return success;

	const int kBufSize = 4096;
	char buf[kBufSize + 1];
	snprintf(buf, kBufSize, "%s %s\n", (success ? "[ OK ]" : "[FAIL]"), expr_str);
	::ax::Debug::_internal_log(buf);
	if (!success) {
		::ax::Debug::_internal_assert("", expr_str, loc, buf);
	}
	return success;
}

} // namespace
