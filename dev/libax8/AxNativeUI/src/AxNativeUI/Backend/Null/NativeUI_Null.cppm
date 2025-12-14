module;

export module AxNativeUI:NativeUI_Null;
export import :NativeUIWindow_Base;
export import :NativeUIApp_Base;

export namespace ax {

class NativeUIWindow_Null;
class NativeUIApp_Null : public NativeUIApp_Base {
	AX_RTTI_INFO(NativeUIApp_Null, NativeUIApp_Base)
public:
	using NativeWindow = NativeUIWindow_Null;
	using CreateDesc = NativeUIApp_CreateDesc;

	NativeUIApp_Null() : Base(CreateDesc()) {}

	virtual int		onRun		() override { return _returnCode; }
	virtual	void	quit		(int returnCode) override { _returnCode = returnCode; }

	virtual bool	onRequestNativeCustomAppEvent() override { return false; }

	static Vec2f	s_getVirtualDesktopSize() { return Vec2f::s_zero(); }
};

class NativeUIWindow_Null : public NativeUIWindow_Base {
	AX_RTTI_INFO(NativeUIWindow_Null, NativeUIWindow_Base)
public:
	NativeUIWindow_Null(CreateDesc& desc) : Base(desc) {}
	void onCreate(CreateDesc& desc) {}
};

} // namespace ax
