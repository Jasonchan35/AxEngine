module;

export module AxImUI:ImUIControls;
export import :Common; 

export namespace ax::ImUI {

constexpr const char* mixedValueFormat = "--";
constexpr const char* floatFormat = "%0.3f";
constexpr bool showMixedValue = false;

bool IsItemClicked();
bool IsKeyShift();
bool IsKeyAlt();
bool IsKeyCtrl();

} // namespace

export namespace ax {

enum class ImUIGizmoOperation {
	None            = 0,
	TranslateX      = (1u << 0),
	TranslateY      = (1u << 1),
	TranslateZ      = (1u << 2),
	RotateX         = (1u << 3),
	RotateY         = (1u << 4),
	RotateZ         = (1u << 5),
	RotateScreen    = (1u << 6),
	ScaleX          = (1u << 7),
	ScaleY          = (1u << 8),
	ScaleZ          = (1u << 9),
	Bounds          = (1u << 10),
	ScaleXU         = (1u << 11),
	ScaleYU         = (1u << 12),
	ScaleZU         = (1u << 13),

	Translate = TranslateX | TranslateY | TranslateZ,
	Rotate    = RotateX    | RotateY    | RotateZ   | RotateScreen,
	Scale     = ScaleX     | ScaleY     | ScaleZ    ,
	ScaleU    = ScaleXU    | ScaleYU    | ScaleZU   , // universal
	Universal = Translate  | Rotate     | ScaleU
};
enum class ImUIGizmoSpace { Local, World };

void ImUISameLine();
void ImUINewLine();

bool ImUIGizmoIsUsing();

bool ImUIGizmoManipulate(	const Mat4f& viewMatrix, const Mat4f& projMatrix,
							ImUIGizmoOperation op, ImUIGizmoSpace space, const Vec3f* snap, 
							const BBox3f& bounds,
							Mat4f& objMatrix,
							Mat4f* outDeltaMatrix = nullptr);

void ImUIGizmoViewManipulate(Mat4f& viewMatrix, const Rect2f& rect);

bool ImUIButton(ZStrView label, Vec2f size);
bool ImUIRadioButton(ZStrView label, bool active);

void ImUILabelText(ZStrView label, ZStrView text);

bool ImUIInputFloat3(ZStrView label, Vec3f& value);
bool ImUIInputFloat3(ZStrView label, Quat4f& value);
bool ImUIInputFloat4(ZStrView label, Quat4f& value);

bool ImUIDragFloat(
		ZStrView label, 
		float* v, 
		float v_speed = 0.1f, 
		float v_min = f32_min,
		float v_max = f32_max);

bool ImUIDragInt(
		ZStrView label, 
		Int* v, 
		float v_speed = 0.1f, 
		Int v_min = i32_min,
		Int v_max = i32_max);

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

} // namespace
