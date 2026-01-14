#pragma once

#include "AxEngine/src/AxEngine/Scene/SceneEntity.cppm"


template<class HANDLER> inline
static void generated_node_types(HANDLER& handler) {
	handler.template addType< AxEngine::SceneComponent                 >();
	handler.template addType< AxEngine::SceneEntity                    >();
	handler.template addType< AxEngine::TransformComponent             >();
	handler.template addType< AxEngine::RenderMeshComponent            >();
}

