#pragma once

#define AX_FORWARD(a) ::std::forward<decltype(a)>(a)

#define AX_ASSERT(EXPR) ::ax::ax_assert((EXPR), #EXPR)
#define AX_ASSERT_MSG(EXPR, MSG) ::ax::ax_assert((EXPR), #EXPR)

#define AX_UNUSED(v)	((void)v)
