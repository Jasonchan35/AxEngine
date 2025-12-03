module;

#include "AxCore-pch.h"
module AxCore.Rtti;
import AxCore.Logger;

namespace ax {
void Rtti::DebugDump() {
	AX_LOG("DebugDump Rtti name = {}", this->name);
	for (auto& field : this->allFields) {
		AX_LOG("  field {} owner=[{}]", field->name, field->fieldOwner->name);
	}
}
} // namespace

