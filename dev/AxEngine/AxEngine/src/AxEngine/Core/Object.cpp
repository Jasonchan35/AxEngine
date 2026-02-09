module;

module AxEngine;
import :Object;

namespace AxEngine {

ObjectManager* ObjectManager::s_instance() {
	static GlobalSingleton<ObjectManager> s;
	return s.ptr();
}

void ObjectManager::_addType(const Rtti* rtti) {
	_types.append(rtti);
}


void ObjectManager::Selection::select(Object* obj) {
	deselectAll();
	obj->editor.selected = true;
	_selectedList.emplaceBack(obj);
}

void ObjectManager::Selection::deselectAll() {
	for (auto& p : _selectedList) {
		if (auto sp = p.getSPtr()) {
			sp->editor.selected = false;
		}
	}
	_selectedList.clear();
}

void ObjectManager::Selection::getSelection(IArray<SPtr<Object>> & outList) {
	outList.clear();
	outList.ensureCapacity(_selectedList.size());
	for (auto& p : _selectedList) {
		if (auto sp = p.getSPtr()) {
			outList.emplaceBack(std::move(sp));
		}
	}
}

SPtr<Object> ObjectManager::Selection::lastSelectedObject() {
	if (_selectedList.size() <= 0) return nullptr;
	return _selectedList.back().getSPtr();
}

} // namespace