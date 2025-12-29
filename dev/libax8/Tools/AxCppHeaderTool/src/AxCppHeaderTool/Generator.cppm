module;
export module AxCppHeaderTool:Generator;
export import :PCH;
export import :Parser;
export import :App;

export namespace ax::AxCppHeaderTool {

class Generator : public NonCopyable {
public:
	Generator();
	void gen(CmdOptions& opt, StrView filename);
	void gen_type(TypeInfo& type);

	Parser	_parser;
	TypeDB	_typeDB;

	String	_outImpl;
};

} //namespace