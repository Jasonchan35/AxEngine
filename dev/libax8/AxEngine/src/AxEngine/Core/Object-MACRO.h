#pragma once

#define AX_CLASS(...)
#define AX_PROP(...)
#define AX_ENUM(...)


#define AX_GENERATED_BODY()  AX_CALL(AX_JOIN_WORD(AX_GENERATED_BODY_LINE, __LINE__), )