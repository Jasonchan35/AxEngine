module;

#include "AxHeaderTool-pch.h"
export module AxHeaderTool.Generator;
export import AxHeaderTool._PCH;
export import AxHeaderTool.Parser;

export namespace ax::AxHeaderTool {

class Generator : public NonCopyable {
public:
	Generator();
	void gen(StrView filename);
	void gen_type(TypeInfo& type);

	Parser	_parser;
	TypeDB	_typeDB;

	String	_outImpl;
};

} //namespace