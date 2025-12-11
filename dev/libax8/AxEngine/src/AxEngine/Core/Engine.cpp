module;

module AxEngine;
import :Engine;

namespace ax::AxEngine{

static Engine* Engine_instance;

Engine* Engine::s_instance() {
	return Engine_instance;
}

Engine::Engine() {
	AX_ASSERT(Engine_instance == nullptr);
	Engine_instance = this;
}

Engine::~Engine() {
	AX_ASSERT(Engine_instance == this);
	Engine_instance = nullptr;
}

void Engine::create(CreateDesc& desc) {
	_inEditor = desc.inEditor;
}

} //namespace
