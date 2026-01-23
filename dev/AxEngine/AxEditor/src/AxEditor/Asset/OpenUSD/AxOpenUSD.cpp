module;

#include "AxOpenUSD-HEADER.h"

module AxEditor;

import :AxOpenUSD;

namespace AxEditor {

AxOpenUSD::AxOpenUSD() {
}

void AxOpenUSD::openFile(StrView filename) {
#if 0
//	System::setEnvVariable("TF_DEBUG", "*");
//	System::setEnvVariable("PXR_PLUGINPATH_NAME", "W:/Jason/github/AxEngine/dev/external/OpenUSD/_build/plugin/usd");
	pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(TempString(filename).c_str());
	if (!stage) {
		throw Error_Undefined(Fmt("cannot load USD file {}", filename));
	}
#endif
	
}

} // namespace
