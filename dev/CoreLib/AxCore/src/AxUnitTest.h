#pragma once

#include "AxCore-pch.h"

import AxCore.Format;
import AxCore.UnitTest;
import AxCore.Logger;

#define	AX_TEST_GROUP( fn )	\
	::ax::__ax_internal_log("====== Group [" #fn "] ============"); \
	void fn(); \
	fn(); \
//----

#define	AX_TEST_FUNC( fn )	\
	::ax::__ax_internal_log("======= Func [" #fn "] ============"); \
	void fn(); \
	fn(); \
//----

#define	AX_TEST_RUN_CASE(func, ...) do { ::ax::UnitTest_RunCase(#func, &func, ##__VA_ARGS__); } while(false);

#define AX_TEST_DUMP(A) \
	do { \
		auto value = A; \
		auto tmpTestMsg = Fmt("{} (value: {})", #A, A); \
		::ax::UnitTest_Validate(bool((A)), tmpTestMsg.c_str()); \
	} while (false) \
//----

#define AX_TEST_CHECK(A) \
	do { \
		auto tmpTestMsg = Fmt("{}  (value: {})", #A, A); \
		::ax::UnitTest_Validate(bool((A)), tmpTestMsg.c_str()); \
	} while (false) \
//----

#define AX_TEST_EQ(A, B) \
	do { \
		auto tmpTestMsg = Fmt("{} == {}  (value: {} == {})", #A, #B, A, B); \
		::ax::UnitTest_Validate(bool((A) == (B)), tmpTestMsg.c_str()); \
	} while (false) \
//----

#define AX_TEST_NOT_EQ(A, B) \
	do { \
		auto tmpTestMsg = Fmt("{} != {}  (value: {} != {})", #A, #B, A, B); \
		::ax::UnitTest_Validate(bool((A) != (B)), tmpTestMsg.c_str()); \
	} while (false) \
//----

#define AX_TEST_ALMOST_EQ(A, B) \
	do { \
		auto tmpTestMsg = Fmt("{} == {}  (value: almostEqual({}, {}))", #A, #B, A, B); \
		::ax::UnitTest_Validate(Math::almostEqual(A, B), tmpTestMsg.c_str()); \
	} while (false) \
//----

#define AX_TEST_NOT_ALMOST_EQ(A, B) \
	do { \
		auto tmpTestMsg = Fmt("{} == {}  (value: !almostEqual({}, {}))", #A, #B, A, B); \
		::ax::UnitTest_Validate(!Math::almostEqual(A, B), tmpTestMsg.c_str()); \
	} while (false) \
//----

#define AX_TEST_GT(A, B) \
	do { \
		auto tmpTestMsg = Fmt("{} > {}  (value: {} > {})", #A, #B, A, B); \
		::ax::UnitTest_Validate(bool((A) > (B)), tmpTestMsg.c_str()); \
	} while (false) \
//----

#define AX_TEST_GTEQ(A, B) \
	do { \
		auto tmpTestMsg = Fmt("{} >= {}  (value: {} >= {})", #A, #B, A, B); \
		::ax::UnitTest_Validate(bool((A) >= (B)), tmpTestMsg.c_str()); \
	} while (false) \
//----

#define AX_TEST_LT(A, B) \
	do { \
		auto tmpTestMsg = Fmt("{} < {}  (value: {} < {})", #A, #B, A, B); \
		::ax::UnitTest_Validate(bool((A) < (B)), tmpTestMsg.c_str()); \
	} while (false) \
//----

#define AX_TEST_LTEQ(A, B) \
	do { \
		auto tmpTestMsg = Fmt("{} <= {}  (value: {} <= {})", #A, #B, A, B); \
		::ax::UnitTest_Validate(bool((A) <= (B)), tmpTestMsg.c_str()); \
	} while (false) \
//----


