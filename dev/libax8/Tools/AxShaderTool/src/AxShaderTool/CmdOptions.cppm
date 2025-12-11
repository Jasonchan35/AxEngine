module;

export module AxShaderTool:CmdOptions;
export import AxRender;

export namespace  ax::AxRender {

struct CmdOptions {
	bool genNinja = false;
#if AX_RENDERER_NULL
	bool genReflect_Null = false;
#endif		
#if AX_RENDERER_VK
	bool genReflect_VK = false;
#endif
#if AX_RENDERER_DX12
	bool genReflect_DX12 = false;
#endif		
	bool genResultInfo = false;
	bool keepUnusedVariable = false;
	bool quiet = false;
	StrView file;
	String out;
	String entry;
	String profile;
	RenderApi api;
	Array<String>	include_dirs;
};

} // namespace 