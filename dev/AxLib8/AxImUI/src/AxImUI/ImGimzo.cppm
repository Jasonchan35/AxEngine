module;

export module AxImUI:ImGizmo;
export import :Common; 

export namespace ax {


enum class ImGizmo_ManipulateType {
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
enum class ImGizmo_Space { Local, World };


ImDrawList* ImGizmo_GetDrawList();

struct ImGizmo_DrawRequest : public NonCopyable {
	Rect2f viewport {0,0,0,0};
	Mat4f  viewMatrix = Mat4f::s_identity();
	Mat4f  projMatrix = Mat4f::s_identity();
	RenderRequest* renderRequest = nullptr;
};

bool ImGizmo_IsUsing();

bool ImGizmo_Manipulate(	ImGizmo_DrawRequest* req,
							ImGizmo_ManipulateType op, ImGizmo_Space space, const Vec3f* snap, 
							const BBox3f& bounds,
							Mat4f& objMatrix,
							Mat4f* outDeltaMatrix = nullptr);

void ImGizmo_ViewManipulate(ImGizmo_DrawRequest* req, const Rect2f& rect);


void ImGizmo_Camera(ImGizmo_DrawRequest* req,
                     Math::Camera3f&       camera,
                     const Mat4f&          cameraWorldMatrix,
                     const ProjectionDesc& projDesc);

void ImGizmo_Cubes(ImGizmo_DrawRequest* req, Span<Mat4f> cubeMatrixArray);
} // namespace