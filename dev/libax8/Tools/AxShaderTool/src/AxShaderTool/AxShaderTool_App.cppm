module;

export module AxShaderTool:App;
export import :CmdOptions;

export namespace ax /*::AxRender*/ {

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