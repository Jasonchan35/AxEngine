module;

export module AxEditor:InspectorUIPanel;
export import :Common;

namespace AxEditor {

class InspectorUIPanel : public NonCopyable {
public:
	void render(RenderRequest* req);
	
	void _renderEntity(SceneEntity* entity);
	void _renderComponent(SceneComponent* comp);
};

} // namespace 
