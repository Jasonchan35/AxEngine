module;


module AxCore.Rtti;
import AxCore.Logger;

namespace ax {
void Rtti::DebugDump() {
	AX_LOG("DebugDump Rtti name = {}", _name);
	for (auto& field : _allFields) {
		AX_LOG("  field {} owner=[{}]", field->_name, field->_fieldOwner->_name);
	}
}
} // namespace

