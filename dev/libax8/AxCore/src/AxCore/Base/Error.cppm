module;

export module AxCore.Error;
export import AxCore.BasicType;

export namespace ax {

class Error : public std::exception {
public:
	Error(std::string_view msg, const SrcLoc& srcLoc);
	Error(const SrcLoc& srcLoc) : Error(std::string_view(), srcLoc) {}

	virtual char const* what() const override { return _what.c_str(); }

	static void s_setEnableAssertion(bool b) { s_enableAssertion = b; }
	static bool s_getEnableAssertion() { return s_enableAssertion; }
	
protected:
	inline static bool s_enableAssertion = false;
	
	std::string _what;
	SrcLoc _srcLoc;
};

AX_SIMPLE_ERROR(Error_Undefined) // TODO: remove it with better error
AX_SIMPLE_ERROR(Error_IndexOutOfRange)
AX_SIMPLE_ERROR(Error_InvalidSize)
AX_SIMPLE_ERROR(Error_BufferOverlapped)
AX_SIMPLE_ERROR(Error_SafeCast)
AX_SIMPLE_ERROR(Error_Allocator)
AX_SIMPLE_ERROR(Error_Format)
AX_SIMPLE_ERROR(Error_Utf)
AX_SIMPLE_ERROR(Error_ParseString)
AX_SIMPLE_ERROR(Error_Time)
AX_SIMPLE_ERROR(Error_File)
AX_SIMPLE_ERROR(Error_Runtime)
AX_SIMPLE_ERROR(Error_JsonWriter)
AX_SIMPLE_ERROR(Error_JsonReader)
AX_SIMPLE_ERROR(Error_JsonValue)

} // namespace 