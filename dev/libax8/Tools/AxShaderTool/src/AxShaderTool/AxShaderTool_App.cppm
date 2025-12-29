module;

export module AxShaderTool:App;
export import AxRender;

export namespace ax /*::AxRender*/ {

struct CmdOptions : public NonCopyable {
	CmdOptions();
	
	static CmdOptions& s_instance();
	
	bool genNinja = false;
#if AX_RENDERER_NULL
	bool genReflect_Null = false;
#endif		
#if AX_RENDERER_VK
	bool genReflect_Vk = false;
#endif
#if AX_RENDERER_DX12
	bool genReflect_Dx12 = false;
#endif		
	bool genResultInfo = false;
	bool keepUnusedVariable = false;
	bool quiet = false;
	StrView file;
	String out;
	String entry;
	String profile;
	RenderAPI api;
	Array<String>	include_dirs;
	
	File::WriteFileOpt	writeFileOpt;
};

class AxShaderTool_App : public ConsoleApp {
	AX_RTTI_INFO(AxShaderTool_App, ConsoleApp)
public:
	AX_DOWNCAST_GET_INSTANCE();
	
	AxShaderTool_App();

	void genNinja_ShadersInFolder	(StrView outDir, StrView filename);
	void genNinja_Shaders			(StrView outDir, const Array<String>& files);
	void genNinja_Shader			(StrView outDir, StrView filename);
	void genNinja_Shader_API		(RenderAPI api, ShaderDeclareInfo& info, StrView outDir, StrView filename);

	void writeNinja_Header			(IString& outStr);
	
	CmdOptions& opt = CmdOptions::s_instance();
	
	void showHelp();
	int onRun() override;

	struct HLSLProfile {
		String	stage;
		Int		major = 0;
		Int		minor = 0;
	};
	
};

} // namespace