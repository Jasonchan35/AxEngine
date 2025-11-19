export module AxCore.UnitTest;

#include "AxBase.h"
import AxCore.String;

export namespace ax {

	class UnitTestClass : public NonCopyable {
	public:
	};
	
	class UnitTestProject : public NonCopyable {
	public:
		virtual ~UnitTestProject() = default;
		virtual void onRun()  = 0;
		int run();
		int waitKeyPress();
	};

} // namespace