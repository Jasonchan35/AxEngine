module;
#include "AxCore-pch.h"

export module AxCore.UnitTest;
export import AxCore.String;

export namespace ax {

struct UnitTestRequest : public NonCopyable {
	// JsonValue	options;
	bool verbose = false;

	AX_NODISCARD ScopeValue<bool> scopedVerbose() {
		return ScopeValue<bool>(&verbose, true);
	}
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

struct UnitTestClass : public NonCopyable {
	static UnitTestRequest& s_testRequest() { return UnitTestProgram::s_get()->testRequest; }
};

inline
bool UnitTest_Validate(bool success, const char* expr_str, const SrcLoc& loc = SrcLoc()) {
	bool verbose = UnitTestProgram::s_get()->testRequest.verbose;

	if (success && !verbose)
		return success;

	const int kBufSize = 4096;
	char buf[kBufSize + 1];
	snprintf(buf, kBufSize, "%s %s", (success ? "[ OK ]" : "[FAIL]"), expr_str);
	__ax_internal_log(buf);
	if (!success) {
		__ax_internal_assert("", expr_str, loc, buf);
	}
	return success;
}

template<class T> inline
const char* _ax_get_cls_name() {
	auto* src = AX_FUNC_SIG;
	auto* t = "_ax_get_cls_name";
	auto* sz = std::strstr(src, t);
	if (!sz) AX_ASSERT(false);
	return sz + std::strlen(t);
}

template<class TEST_CLASS, class... FUNC_ARGS, class... CALL_ARGS>
inline void UnitTest_RunCase(const char* funcName, void (TEST_CLASS::*testFunc)(FUNC_ARGS...), CALL_ARGS&&... args) {
//	const char* clsName = _ax_get_cls_name<TEST_CLASS>();
	const int kBufSize = 4096;
	char msg[kBufSize + 1];
	snprintf(msg, kBufSize, "---- RunCase [%s] ---------", funcName);
	__ax_internal_log(msg);

	try {
		TEST_CLASS obj;		
		(obj.*testFunc)(AX_FORWARD(args)...);

	} catch (const Error& e) {
		snprintf(msg, kBufSize, "\n  uncatched ax::Error occur %s", e.what());
		__ax_internal_log(msg);
		AX_ASSERT(false);

	} catch (const std::exception& e) {
		snprintf(msg, kBufSize, "\n  uncatched std::exception occur %s", e.what());
		__ax_internal_log(msg);
		AX_ASSERT(false);

	} catch (...) {
		snprintf(msg, kBufSize, "\n  uncatched exception occur");
		__ax_internal_log(msg);
		AX_ASSERT(false);
	}
	//	TestClass o;
	//	o.TestFunc;
}

} // namespace
