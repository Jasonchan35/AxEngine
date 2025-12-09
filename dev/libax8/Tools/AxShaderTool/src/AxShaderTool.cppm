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

	void writeNinja_VulkanPass		(IString& outStr, IArray<String> & outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename);
	void writeNinja_DX12Pass		(IString& outStr, IArray<String> & outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename);


	void showHelp();
	int onRun() override;

	struct Option {
		bool genNinja = false;
		bool genReflect_VK = false;
		bool genReflect_DX12 = false;
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