module;

export module AxNinjaBuild;
export import AxCore.Logger;

export namespace ax {

struct AxNinjaBuild {
	AxNinjaBuild() = delete;
	static TempString escapeString(StrView str);
};

class AxNinjaBuildWriter : public NonCopyable {
public:
	
};

} // namespace 
