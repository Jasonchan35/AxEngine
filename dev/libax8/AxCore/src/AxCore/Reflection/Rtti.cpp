module;


module AxCore.Rtti;
import AxCore.Logger;

namespace ax {

void Rtti::debugDump() const {
	AX_LOG("DebugDump Rtti name = {}", name);
	for (auto& field : allFields) {
		AX_LOG("  field {} offset={} owner=[{}] ", field->name, field->offset, field->fieldOwner->name);
	}
}

} // namespace

