module;

export module AxShaderTool:App;
export import :CmdOptions;

export namespace ax /*::AxRender*/ {

class AxShaderTool_App : public ConsoleApp {
	using This = AxShaderTool_App;
public:
	AxShaderTool_App();

	void genNinja_ShadersInFolder	(StrView outDir, StrView filename);
	void genNinja_Shaders			(StrView outDir, const Array<String>& files);
	void genNinja_Shader			(StrView outDir, StrView filename);
	void genNinja_Shader_API		(RenderAPI api, ShaderDeclareInfo& info, StrView outDir, StrView filename);

	void writeNinja_Header			(IString& outStr);

#if AX_RENDERER_NULL
	void writeNinja_NullPass		(IString& outStr, IArray<String> & outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename);
#endif
	
#if AX_RENDERER_VK
	void writeNinja_VkPass			(IString& outStr, IArray<String> & outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename);
#endif
#if AX_RENDERER_DX12
	void writeNinja_Dx12Pass		(IString& outStr, IArray<String> & outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename);
#endif

	void showHelp();
	int onRun() override;

	CmdOptions opt {};

	struct HLSLProfile {
		String	stage;
		Int		major = 0;
		Int		minor = 0;
	};
	
};

} // namespace