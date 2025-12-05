module;
#include "AxHeaderTool-pch.h"
export module AxHeaderTool.TypeInfo;
export import AxHeaderTool._PCH;

export namespace ax::AxHeaderTool {

inline
void convertToDisplayName(IString& outStr, StrView s) {
	outStr.clear();
	outStr.reserve(s.size());

	Char last = 0;
	for (auto ch : s) {
		if (std::islower(last) && std::isupper(ch)) {
			outStr.append(' ');
		}
		if (ch == '_')
			ch = ' ';
		outStr.append(ch);

		last = ch;
	}

	if (outStr) {
		outStr[0] = static_cast<Char>(CharUtil::toUpper(outStr[0]));
	}
}

class Attribute {
public:
	String	name;
	String	defaultValue;
};

enum class PropType {
	Unknown,
	Normal,
	DiaInput,
	DiaOutput,
};

class PropInfo : public NonCopyable {
public:
	PropType propType = PropType::Unknown;
	String name;
	String displayName;
	String varName;
	String typeName;
	String defaultValue;

	struct HasAttr {
		bool dontSave = false;
	};
	HasAttr		hasAttr;
	Array<Attribute>	attributes;
};

class TypeInfo : public NonCopyable {
public:
	String name;
	String baseName;
	String fullname;
	String openNamespaceScope;
	String closeNamespaceScope;
	Dict<String, PropInfo>	props;
};

class TypeDB : public NonCopyable {
public:
	TypeDB();
	~TypeDB();

	Dict<String, TypeInfo>	types;
};


} //namespace axNodeGen