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

} // namespace
