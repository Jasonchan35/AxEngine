module;

module AxCore.Error;
import AxCore.Logger;

namespace ax {

Error::Error(std::string_view msg, const SrcLoc& srcLoc): _what(msg), _srcLoc(srcLoc) {
	AX_LOG_ERROR("ax::Error: {}", msg);
	AX_ASSERT(false);
}

} // namespace

