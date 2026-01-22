module;

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>

module AxEditor;
import :AxOpenUsd;

namespace AxEditor {

AxOpenUsd::AxOpenUsd() {
}

void AxOpenUsd::openFile(StrView filename) {
	System::setEnvVariable("TF_DEBUG", "PLUG_INFO_SEARCH");
	
	System::setEnvVariable("PXR_PLUGINPATH_NAME", 
		"W:/Jason/github/AxEngine/dev/_vcpkg/installed/x64-windows/bin/usd/usd/resources");

	
	pxr::ArResolverContext resolverContext;
	
	pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(TempString(filename).c_str(), resolverContext);
	if (!stage) {
		throw Error_Undefined(Fmt("cannot load USD file {}", filename));
	}
	
	
}

} // namespace



