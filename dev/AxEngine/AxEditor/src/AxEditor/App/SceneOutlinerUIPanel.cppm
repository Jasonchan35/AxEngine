module;

export module AxEditor:SceneOutlinerUIPanel;
export import :Common;

namespace AxEditor {

class SceneOutlinerUIPanel : public NonCopyable {
public:
	void render(SceneWorld* world, RenderRequest* req);
	
private:
	void _addNode(SceneEntity* p);
};

} // namespace 
