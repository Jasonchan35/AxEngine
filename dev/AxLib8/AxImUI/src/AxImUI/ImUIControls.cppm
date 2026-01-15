module;

export module AxImUI:ImUIControls;
export import :Common; 

export namespace AxUI {
	constexpr const char* mixedValueFormat = "--";
	constexpr const char* floatFormat = "%0.3f";
	constexpr bool showMixedValue = false;

	void ImUILabelText(ZStrView label, ZStrView text);

	bool ImUIDragFloat(
			ZStrView label, 
			float* v, 
			float v_speed = 0.1f, 
			float v_min = f32_min,
			float v_max = f32_max);

	float ImUIInputFloat(ZStrView label, float* v);

	class ImUIPanel : public NonCopyable {
	public:
		AX_NODISCARD ImUIPanel(ZStrView name, bool* pOpen = nullptr);
		~ImUIPanel();
	};

	struct ImUITreeNodeFlags {
		ImUITreeNodeFlags() 
		: open(false)
		, hasChild(false)
		, selected(false) 
		{}
		bool open     : 1;
		bool hasChild : 1;
		bool selected : 1;
	};

	class ImUITreeNode : public NonCopyable {
	public:
		ImUITreeNode(ZStrView label, ImUITreeNodeFlags flags = {});
		~ImUITreeNode();

		bool isOpen() const { return _isOpen; }

	private:
		bool  _isOpen = true;
	};

	class ImUICollapsingHeader : public NonCopyable {
	public:
		ImUICollapsingHeader(ZStrView label);

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
