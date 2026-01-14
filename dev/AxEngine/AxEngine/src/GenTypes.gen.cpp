module AxEngine;
import :SceneEntity;

void registerTypes() {
	auto* mgr = AxEngine::ObjectManager::s_instance();
	mgr->addType<AxEngine::SceneComponent                >();
	mgr->addType<AxEngine::SceneEntity                   >();
	mgr->addType<AxEngine::TransformComponent            >();
	mgr->addType<AxEngine::RenderMeshComponent           >();
}

