module;

export module AxEditor:InspectorUIPanel;
export import :Common;

namespace AxEditor {

class InspectorUIPanel : public NonCopyable {
public:
	void render(RenderRequest* req);
	
};

} // namespace 
