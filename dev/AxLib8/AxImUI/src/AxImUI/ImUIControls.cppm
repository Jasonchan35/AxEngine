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

void ImUISeparator();
void ImUISameLine();
void ImUINewLine();

bool ImUIGizmoIsUsing();

bool ImUIGizmoManipulate(	const Mat4f& viewMatrix, const Mat4f& projMatrix,
							ImUIGizmoOperation op, ImUIGizmoSpace space, const Vec3f* snap, 
							const BBox3f& bounds,
							Mat4f& objMatrix,
							Mat4f* outDeltaMatrix = nullptr);

void ImUIGizmoViewManipulate(Mat4f& viewMatrix, const Rect2f& rect);

ImDrawList* ImUIGetDrawList();

void ImUIGizmoCamera(const Rect2f&         viewport,
                     const Mat4f&          viewMatrix,
                     const Mat4f&          projMatrix,
                     Math::Camera3f&       camera,
                     const Mat4f&          cameraWorldMatrix,
                     const ProjectionDesc& projDesc);

bool ImUIButton(ZStrView label, Vec2f size);
bool ImUIRadioButton(ZStrView label, bool active);

bool ImUIColorButton(ZStrView label, const Color4f& color);

bool ImUICheckBox(ZStrView label, bool& b);

template<class FLAG> inline
bool ImUICheckBoxFlag(ZStrView label, FLAG& value, FLAG mask) {
	bool b = ax_bit_has(value, mask);
	if (ImUICheckBox(label, b)) {
		value = ax_bit_set(value, mask, b);
		return true;
	}
	return false;
}


template<class T>
struct ImUICheckBoxArray_Item {
	ZStrView name;
	T value;
};

ImDrawList* ImUIGetWindowDrawList();

void ImUIText(ZStrView text);

void ImUILabelText(ZStrView label, ZStrView text);

bool ImUIInputFloat2(ZStrView label, Vec2f& value);
bool ImUIInputFloat3(ZStrView label, Vec3f& value);
bool ImUIInputFloat4(ZStrView label, Vec4f& value);

bool ImUIInputEulerDeg(ZStrView label, Quat4f& value);
bool ImUIInputQuat4(ZStrView label, Quat4f& value);

bool ImUIInputMat4(ZStrView label, Mat4f& value);

bool ImUIDragFloat(
		ZStrView label, 
		float* v, 
		float v_speed = 0.1f, 
		float v_min = f32_min,
		float v_max = f32_max);

template<class T> requires(!std::is_same_v<T, f32>) 
inline bool ImUIDragFloat(ZStrView label, T* v, float v_speed, T v_min, T v_max) {
	f32 tmpV   = ax_safe_cast_from(*v);
	f32 tmpMin = ax_safe_cast_from(v_min);
	f32 tmpMax = ax_safe_cast_from(v_max);
	
	if (ImUIDragFloat(label, &tmpV, v_speed, tmpMin, tmpMax)) {
		*v = tmpV;
		return true;
	}
	return false;
}


bool ImUIDragInt(
		ZStrView label, 
		i32* v, 
		float v_speed = 0.1f, 
		i32 v_min = i32_min,
		i32 v_max = i32_max);

template<class T> requires(!std::is_same_v<T, i32>) 
inline bool ImUIDragInt(ZStrView label, T* v, float v_speed, T v_min, T v_max) {
	int tmpV   = ax_safe_cast_from(*v);
	int tmpMin = ax_safe_cast_from(v_min);
	int tmpMax = ax_safe_cast_from(v_max);
	
	if (ImUIDragInt(label, &tmpV, v_speed, tmpMin, tmpMax)) {
		*v = tmpV;
		return true;
	}
	return false;
}

float ImUIInputFloat(ZStrView label, float* v);

template<class T> inline
bool ImUICheckBoxArray(ZStrView label, T& value, Span<ImUICheckBoxArray_Item<T>> list) {
	ImUIText(label);
	bool changed = false;
	int i = 0;
	for (auto& s : list) {
		bool v = value == s.value;
		if (i > 0) {
			ImUISameLine();
		}
		if (ImUICheckBox(s.name, v)) {
			value = s.value;
			changed = true;
		}
		
		i++;
	}
	return changed;
}


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
