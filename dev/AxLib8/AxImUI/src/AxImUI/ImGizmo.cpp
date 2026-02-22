module;

module AxImUI;
import :ImGizmo;

namespace ax {

bool ImGizmo_IsUsing() { return ImGuizmo::IsUsing(); }

ImDrawList* ImGizmo_GetDrawList() { return ImGuizmo::_AxImUIGetDrawList(); }

bool ImGizmo_Manipulate(ImGizmo_DrawRequest*   req,
                        ImGizmo_ManipulateType op,
                        ImGizmo_Space          space,
                        const Vec3f*           snap,
                        const BBox3f&          bounds,
                        Mat4f&                 objMatrix,
                        Mat4f*                 outDeltaMatrix) {
	const float* pSnap      = snap ? snap->e : nullptr;
	bool         opIsBounds = op == ImGizmo_ManipulateType::Bounds;

	float localBounds[6];
	localBounds[0] = bounds.min.x;
	localBounds[1] = bounds.min.y;
	localBounds[2] = bounds.min.z;
	localBounds[3] = bounds.max.x;
	localBounds[4] = bounds.max.y;
	localBounds[5] = bounds.max.z;

	bool hasLocalBounds = opIsBounds && bounds.isValid();

	bool ret = ImGuizmo::Manipulate(req->viewMatrix.e,
	                                req->projMatrix.e,
	                                static_cast<ImGuizmo::OPERATION>(op),
	                                static_cast<ImGuizmo::MODE>(space),
	                                objMatrix.e,
	                                outDeltaMatrix ? outDeltaMatrix->e : nullptr,
	                                opIsBounds ? nullptr : pSnap,
	                                hasLocalBounds ? localBounds : nullptr,
	                                opIsBounds ? pSnap : nullptr);
	return ret;
}

void ImGizmo_ViewManipulate(ImGizmo_DrawRequest* req, const Rect2f& rect) {
	float  distance = 50;
	ImVec2 pos(rect.x, rect.y);
	ImVec2 size(rect.w, rect.h);
	ImU32  bgColor = 0x00000000;
	ImGuizmo::ViewManipulate(req->viewMatrix.e, distance, pos, size, bgColor);
}

void ImGizmo_Camera(ImGizmo_DrawRequest*  req,
                    Math::Camera3f&       camera,
                    const Mat4f&          cameraWorldMatrix,
                    const ProjectionDesc& projDesc) {
	ImDrawList* drawList = ImGizmo_GetDrawList();

	FixedArray<Vec3f, 8> points;
	points = camera.getFrustumPoints(projDesc, cameraWorldMatrix);

	auto                  matMVP = req->projMatrix * req->viewMatrix;
	FixedArray<ImVec2, 8> screenPoints;

	for (Int i = 0; i < 8; i++) {
		auto clipSpace  = matMVP.mulPoint(points[i]);
		auto pt         = clipSpace.xy() * Vec2f(0.5f, -0.5f) + 0.5f;
		pt              = pt * req->viewport.size - req->viewport.pos;
		screenPoints[i] = ImVec2_make(pt);
	}

	ImU32           color     = 0xffffffff;
	constexpr float thickness = -1;

	drawList->AddLine(screenPoints[0], screenPoints[1], color, thickness);
	drawList->AddLine(screenPoints[1], screenPoints[2], color, thickness);
	drawList->AddLine(screenPoints[2], screenPoints[3], color, thickness);
	drawList->AddLine(screenPoints[3], screenPoints[0], color, thickness);

	drawList->AddLine(screenPoints[4], screenPoints[5], color, thickness);
	drawList->AddLine(screenPoints[5], screenPoints[6], color, thickness);
	drawList->AddLine(screenPoints[6], screenPoints[7], color, thickness);
	drawList->AddLine(screenPoints[7], screenPoints[4], color, thickness);

	drawList->AddLine(screenPoints[0], screenPoints[4], color, thickness);
	drawList->AddLine(screenPoints[1], screenPoints[5], color, thickness);
	drawList->AddLine(screenPoints[2], screenPoints[6], color, thickness);
	drawList->AddLine(screenPoints[3], screenPoints[7], color, thickness);
}

void ImGizmo_Cubes(ImGizmo_DrawRequest* req, Span<Mat4f> cubeMatrixArray) {
	ImGuizmo::DrawCubes(req->viewMatrix.e,
	                    req->projMatrix.e,
	                    cubeMatrixArray.data()->e,
	                    ax_safe_cast_from(cubeMatrixArray.size()));
}

} // namespace 