module;

export module AxEditor:InspectorUIPanel;
export import :Common;

namespace AxEditor {

class InspectorUIPanel : public NonCopyable {
public:
	void render(RenderRequest* req);
	void _renderEntity(ImUI_InspectorRequest* req, SceneEntity* entity);
	void _renderComponent(ImUI_InspectorRequest* req, SceneComponent* comp);
};

} // namespace 
