module;

#include "pxr/usd/usd/stage.h"

module AxEditor;
import :AxOpenUsd;

namespace AxEditor {

AxOpenUsd::AxOpenUsd() {
}

void AxOpenUsd::openFile(StrView filename) {
	
	pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(TempString(filename).c_str());
	AX_UNUSED( stage );	
}

} // namespace



