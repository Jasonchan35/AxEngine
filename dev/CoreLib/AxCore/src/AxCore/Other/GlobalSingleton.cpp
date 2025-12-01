module AxCore.GlobalSingleton;

namespace  ax {

void GlobalSingletonManager::add(GlobalSingletonBase* p) {
	auto data = _data.scopedLock();
	data->list.append(UPtr_ref(p));
}

void GlobalSingletonManager::shutdown() {
	auto data = _data.scopedLock();

	while(auto p = data->list.popTail()) {
		p->callDestructor();
		p.detach();
	}
}

GlobalSingletonManager* GlobalSingletonManager::s_instance() {
	static GlobalSingletonManager s;
	return &s;
}

} // namespace

