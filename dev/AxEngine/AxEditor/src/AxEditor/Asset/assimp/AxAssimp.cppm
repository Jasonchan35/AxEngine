module;

export module AxEditor:AxAssimp;
export import :Common;

export namespace AxEditor {

class AxAssimp : public NonCopyable {
public:
	SPtr<SceneWorld> openFile(StrView filename);
};

} // namespace
