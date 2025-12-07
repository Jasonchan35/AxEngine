module AxRender.ShaderInfo;
import AxRender.Vertex;

namespace ax::AxRender {

void ShaderStageInfo::loadFile(StrView filename_) {
	filename = filename_;
	JsonIO::readFile(filename_, *this);
}

const String& ShaderPassInfo::getFuncName(ShaderStageFlags mask) const {
	switch (mask) {
		case ShaderStageFlags::Vertex:		return vsFunc;
		case ShaderStageFlags::Pixel:		return psFunc;
		case ShaderStageFlags::Geometry:	return gsFunc;
		case ShaderStageFlags::Compute:		return csFunc;
		default: return String::kEmpty();
	}
}

ShaderPassInfo* ShaderDeclareInfo::findPass(StrView name) {
	for (auto& p : passes) {
		if (p.name == name) return &p;
	}
	return nullptr;
}

} // namespace 
