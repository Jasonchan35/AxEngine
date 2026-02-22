module;

export module AxImUI:ImUIControls;
export import :Common; 

export namespace ax {

constexpr const char* ImUI_mixedValueFormat = "--";
constexpr const char* ImUI_floatFormat = "%0.3f";
constexpr bool ImUI_showMixedValue = false;

bool ImUI_IsItemClicked();
bool ImUI_IsKeyShift();
bool ImUI_IsKeyAlt();
bool ImUI_IsKeyCtrl();

void ImUI_Separator();
void ImUI_SameLine();
void ImUI_NewLine();


bool ImUI_Button(ZStrView label, Vec2f size);
bool ImUI_RadioButton(ZStrView label, bool active);

bool ImUI_ColorButton(ZStrView label, const Color4f& color);

bool ImUI_CheckBox(ZStrView label, bool& b);

template<class FLAG> inline
bool ImUI_CheckBoxFlag(ZStrView label, FLAG& value, FLAG mask) {
	bool b = ax_bit_has(value, mask);
	if (ImUI_CheckBox(label, b)) {
		value = ax_bit_set(value, mask, b);
		return true;
	}
	return false;
}


template<class T>
struct ImUI_CheckBoxArray_Item {
	ZStrView name;
	T value;
};

ImDrawList* ImUI_GetWindowDrawList();

void ImUI_Text(ZStrView text);

void ImUI_LabelText(ZStrView label, ZStrView text);

bool ImUI_InputFloat2(ZStrView label, Vec2f& value);
bool ImUI_InputFloat3(ZStrView label, Vec3f& value);
bool ImUI_InputFloat4(ZStrView label, Vec4f& value);

bool ImUI_InputEulerDeg(ZStrView label, Quat4f& value);
bool ImUI_InputQuat4(ZStrView label, Quat4f& value);

bool ImUI_InputMat4(ZStrView label, Mat4f& value);

bool ImUI_DragFloat(
		ZStrView label, 
		float& v, 
		float v_speed = 0.1f, 
		float v_min = f32_min,
		float v_max = f32_max);

template<class T> requires(!std::is_same_v<T, f32>) 
inline bool ImUI_DragFloat(ZStrView label, T& v, float v_speed, T v_min, T v_max) {
	f32 tmpV   = ax_safe_cast_from(*v);
	f32 tmpMin = ax_safe_cast_from(v_min);
	f32 tmpMax = ax_safe_cast_from(v_max);
	
	if (ImUI_DragFloat(label, tmpV, v_speed, tmpMin, tmpMax)) {
		*v = tmpV;
		return true;
	}
	return false;
}

bool ImUI_DragFloat3(ZStrView label,
                    Vec3f&   value,
                    float    v_speed = 0.1f,
                    float    v_min   = -1000.0f,
                    float    v_max   = +1000.0f);

bool ImUI_DragEuler(ZStrView label,
					Quat4f&   value,
					float    v_speed = 0.1f,
					float    v_min   = -1000.0f,
					float    v_max   = +1000.0f);


bool ImUI_DragInt(
		ZStrView label, 
		i32& v, 
		float v_speed = 0.1f, 
		i32 v_min = i32_min,
		i32 v_max = i32_max);

template<class T> requires(!std::is_same_v<T, i32>) 
inline bool ImUI_DragInt(ZStrView label, T& v, float v_speed, T v_min, T v_max) {
	int tmpV   = ax_safe_cast_from(*v);
	int tmpMin = ax_safe_cast_from(v_min);
	int tmpMax = ax_safe_cast_from(v_max);
	
	if (ImUI_DragInt(label, tmpV, v_speed, tmpMin, tmpMax)) {
		v = tmpV;
		return true;
	}
	return false;
}

float ImUI_InputFloat(ZStrView label, float& v);

template<class T> inline
bool ImUI_CheckBoxArray(ZStrView label, T& value, Span<ImUI_CheckBoxArray_Item<T>> list) {
	ImUI_Text(label);
	bool changed = false;
	int i = 0;
	for (auto& s : list) {
		bool v = value == s.value;
		if (i > 0) {
			ImUI_SameLine();
		}
		if (ImUI_CheckBox(s.name, v)) {
			value = s.value;
			changed = true;
		}
		
		i++;
	}
	return changed;
}


class ImUI_Panel : public NonCopyable {
public:
	AX_NODISCARD ImUI_Panel(ZStrView name, bool* pOpen = nullptr);
	~ImUI_Panel();
};

struct ImUI_TreeNodeFlags {
	ImUI_TreeNodeFlags() 
	: open(false)
	, hasChild(false)
	, selected(false) 
	{}
	bool open     : 1;
	bool hasChild : 1;
	bool selected : 1;
};

class ImUI_TreeNode : public NonCopyable {
public:
	ImUI_TreeNode(ZStrView label, ImUI_TreeNodeFlags flags = {});
	~ImUI_TreeNode();

	bool isOpen() const { return _isOpen; }

private:
	bool  _isOpen = true;
};

class ImUI_CollapsingHeader : public NonCopyable {
public:
	ImUI_CollapsingHeader(ZStrView label);

private:
	bool _visiable = true;
};

class ImUI_ScopedPushId : public NonCopyable {
public:
	ImUI_ScopedPushId(const void* id);
	ImUI_ScopedPushId(int id);
	~ImUI_ScopedPushId();
};

} // namespace
