module;

export module AxEditor:SceneOutlinerUIPanel;
export import :Common;

namespace AxEditor {

class SceneOutlinerUIPanel : public NonCopyable {
public:
	void render(RenderRequest* req);
};

} // namespace 
