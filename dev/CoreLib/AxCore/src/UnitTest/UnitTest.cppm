export module AxCore.UnitTest;

import AxCore.Base;

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