#pragma once

#define AX_SIZE_OF(T)  ax_safe_cast_Int(sizeof(T))
#define AX_ALIGN_OF(T) ax_safe_cast_Int(alignof(T))

#define AX_FORWARD(a) ::std::forward<decltype(a)>(a)

#define AX_ASSERT(EXPR) ::ax::ax_assert((EXPR), #EXPR)
#define AX_ASSERT_MSG(EXPR, MSG) ::ax::ax_assert((EXPR), #EXPR)

#define AX_UNUSED(v)	((void)v)
