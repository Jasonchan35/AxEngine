module;

export module AxCore:Color;
export import :ColorUtil;
export import :ColorA;
export import :ColorBGRA;
export import :ColorDXT;
export import :ColorHSVA;
export import :ColorLA;
export import :ColorPacked;
export import :ColorRGBA;

export namespace ax {

using Color3b = ColorRGBb;
using Color3s = ColorRGBs;
using Color3h = ColorRGBh;
using Color3f = ColorRGBf;
using Color3d = ColorRGBd;

using Color4b = ColorRGBAb;
using Color4s = ColorRGBAs;
using Color4h = ColorRGBAh;
using Color4f = ColorRGBAf;
using Color4d = ColorRGBAd;
	
} // namespace