module;
#include "AxCore-pch.h"

module AxCore.UnitTest;

namespace ax {

UnitTestProgram* UnitTestProgram_instance = nullptr;

UnitTestProgram::UnitTestProgram() {
	UnitTestProgram_instance = this;
}

UnitTestProgram* UnitTestProgram::s_get() {
	return UnitTestProgram_instance;
}

int UnitTestProgram::run() {
	onRun();

	::ax::Debug::_internal_log("\n\n==== Program Ended ==== \n");

#if 1 // AX_COMPILER_VC
	if (Debug::isDebuggerPresent()) {
		// waitKeyPress(); // doesn't work on Rider
	}
#endif
	return 0;
}

int UnitTestProgram::waitKeyPress() {
	::ax::Debug::_internal_log(" ! Press Any Key to Exit\n\n");
	
#if AX_OS_WINDOWS
	return _getch();
#else
	struct termios oldattr, newattr;
	int            ch;
	tcgetattr(STDIN_FILENO, &oldattr);
	newattr = oldattr;
	newattr.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	return ch;
#endif
}

} // namespace
