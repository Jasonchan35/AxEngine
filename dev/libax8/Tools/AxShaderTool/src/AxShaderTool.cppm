module;

export module AxShaderTool;
export import AxRender;

export namespace ax::AxRender {

class AxShaderTool : public ConsoleApp {
	using This = AxShaderTool;
public:
	AxShaderTool();

	void genNinja_ShadersInFolder	(StrView outdir, StrView filename);
	void genNinja_Shaders			(StrView outdir, const Array<String>& files);
	void genNinja_Shader			(StrView outdir, StrView filename);
	void genNinja_Shader_API		(RenderApi api, ShaderDeclareInfo& info, StrView outdir, StrView filename);

	void writeNinja_Header			(IString& outStr);

#if AX_RENDERER_VK
	void writeNinja_VulkanPass		(IString& outStr, IArray<String> & outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename);
#endif
#if AX_RENDERER_DX12
	void writeNinja_DX12Pass		(IString& outStr, IArray<String> & outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename);
#endif

	void showHelp();
	int onRun() override;

	struct Option {
		bool genNinja = false;
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
	Option opt;

	struct HLSLProfile {
		String	stage;
		Int		major = 0;
		Int		minor = 0;
	};
};

} // namespace