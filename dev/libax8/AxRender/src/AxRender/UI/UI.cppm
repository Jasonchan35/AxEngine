module;

export module AxRender:UI;
export import :Common; 

export namespace ax /*::AxRender*/::UI {
	constexpr const char* mixedValueFormat = "--";
	constexpr const char* floatFormat = "%0.3f";
	constexpr bool showMixedValue = false;

	bool DragFloat(
			StrView label, 
			float* v, 
			float v_speed = 0.1f, 
			float v_min = f32_min,
			float v_max = f32_max);

	float InputFloat(StrView label, float* v);

	class Window : public NonCopyable {
	public:
		Window(StrView name, bool* pOpen = nullptr);
		~Window();
	};

	class TreeNode : public NonCopyable {
	public:
		TreeNode(StrView label);
		~TreeNode();

		bool isOpen() const { return _isOpen; }

	private:
		bool  _isOpen = true;
	};

	class CollapsingHeader : public NonCopyable {
	public:
		CollapsingHeader(StrView label);

	private:
		bool _visiable = true;
	};

	class PushID : public NonCopyable {
	public:
		PushID(const void* id);
		PushID(int id);
		~PushID();
	};

	bool IsItemClicked();

	bool IsKeyShift();
	bool IsKeyAlt();
	bool IsKeyCtrl();


} // namespace
