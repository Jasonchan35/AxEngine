module;

export module AxImUI:ImUIControls;
export import :Common; 

export namespace AxUI {
	constexpr const char* mixedValueFormat = "--";
	constexpr const char* floatFormat = "%0.3f";
	constexpr bool showMixedValue = false;

	bool ImUIDragFloat(
			StrView label, 
			float* v, 
			float v_speed = 0.1f, 
			float v_min = f32_min,
			float v_max = f32_max);

	float ImuiInputFloat(StrView label, float* v);

	class ImUIPanel : public NonCopyable {
	public:
		AX_NODISCARD ImUIPanel(StrView name, bool* pOpen = nullptr);
		~ImUIPanel();
	};

	class ImUITreeNode : public NonCopyable {
	public:
		ImUITreeNode(StrView label);
		~ImUITreeNode();

		bool isOpen() const { return _isOpen; }

	private:
		bool  _isOpen = true;
	};

	class ImUICollapsingHeader : public NonCopyable {
	public:
		ImUICollapsingHeader(StrView label);

	private:
		bool _visiable = true;
	};

	class ImUIPushID : public NonCopyable {
	public:
		ImUIPushID(const void* id);
		ImUIPushID(int id);
		~ImUIPushID();
	};

	bool IsItemClicked();

	bool IsKeyShift();
	bool IsKeyAlt();
	bool IsKeyCtrl();


} // namespace
