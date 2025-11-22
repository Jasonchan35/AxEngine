#pragma once

#include "AxBase.h"

import AxCore.Format;
import AxCore.Debug;
import AxCore.UnitTest;

#define	AX_TEST_GROUP( fn )	\
	::ax::Debug::_internal_log("========================== [" #fn "] ==================\n"); \
	void fn(); \
	fn(); \
//----

#define	AX_TEST_FUNC( fn )	\
	::ax::Debug::_internal_log("[" #fn "]\n"); \
	void fn(); \
	fn(); \
//----

