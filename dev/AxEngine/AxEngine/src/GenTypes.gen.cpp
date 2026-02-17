module AxEngine;
import :Common;

import :SceneEntity;

void registerTypes() {
	auto* mgr = AxEngine::ObjectManager::s_instance();
	AX_UNUSED(mgr);
	mgr->addType<AxEngine::SceneComponent                >();
	mgr->addType<AxEngine::SceneEntity                   >();
	mgr->addType<AxEngine::MeshRendererComponent         >();
	mgr->addType<AxEngine::LightComponent                >();
	mgr->addType<AxEngine::CameraComponent               >();
	mgr->addType<AxEngine::SceneWorld                    >();
}

