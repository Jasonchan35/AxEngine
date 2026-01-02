module;

#if AX_NATIVE_UI_WIN32

export module AxNativeUI:NativeUI_Win32;
export import :NativeUIEvent;

export namespace ax {

constexpr UINT AX_WM_USER_HasCustomAppEvent = WM_USER + 1;

class NativeUI_Win32 {
public:

	static NativeUIKeyCode	 s_toNativeKey(int key);
	static int				 s_toWin32Key(NativeUIKeyCode key);
	
	static NativeUIEventModifier s_eventModifier();

	static Vec2f  s_worldToLocalPos(HWND hwnd, const Vec2f& pt);
	static Vec2f  s_localToWorldPos(HWND hwnd, const Vec2f& pt);

	static Rect2f s_getWorldRect(HWND hwnd);
	static Rect2f s_getLocalRect(HWND hwnd);

	template<class T>
	AX_NODISCARD AX_INLINE static constexpr Vec2_<T> to_Vec2_(const ::POINT& r) {
		auto tmp = Vec2_<LONG>(r.x, r.y);
		return Vec2_<T>::s_cast(tmp);
	}
	
	AX_NODISCARD AX_INLINE static constexpr Vec2i to_Vec2i(const ::POINT& r) { return to_Vec2_<Int>(r); }
	AX_NODISCARD AX_INLINE static constexpr Vec2f to_Vec2f(const ::POINT& r) { return to_Vec2_<f32>(r); }
	AX_NODISCARD AX_INLINE static constexpr Vec2d to_Vec2d(const ::POINT& r) { return to_Vec2_<f64>(r); }

	template<class T>
	AX_NODISCARD AX_INLINE static constexpr POINT to_POINT(const Vec2_<T>& src) {
		auto tmp = Vec2_<LONG>::s_cast(src);
		return POINT({.x = tmp.x, .y = tmp.y});
	}
	
	template<class T>
	AX_NODISCARD AX_INLINE static constexpr Rect2_<T> to_Rect2_(const ::RECT& src) {
		auto tmp = Rect2_<LONG>(src.left,
								src.top, 
								src.right  - src.left, 
								src.bottom - src.top);
		return Rect2_<T>::s_cast(tmp);
	}
	
	AX_NODISCARD AX_INLINE static constexpr Rect2i to_Rect2i(const ::RECT& src) { return to_Rect2_<Int>(src); }
	AX_NODISCARD AX_INLINE static constexpr Rect2f to_Rect2f(const ::RECT& src) { return to_Rect2_<f32>(src); }
	AX_NODISCARD AX_INLINE static constexpr Rect2d to_Rect2d(const ::RECT& src) { return to_Rect2_<f64>(src); }

	template<class T>
	AX_NODISCARD AX_INLINE constexpr RECT to_RECT(const Rect2_<T>& src) const {
		auto tmp = Rect2_<LONG>::s_cast(src);
		return RECT({
			.left = tmp.xMin(), .right  = tmp.xMax(),
			.top  = tmp.yMin(), .bottom = tmp.yMax(),
		});
	}
};


} // namespace ax

#endif // #if AX_NATIVE_UI_WIN32