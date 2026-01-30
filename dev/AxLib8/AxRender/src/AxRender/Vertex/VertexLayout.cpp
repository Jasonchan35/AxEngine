module AxRender;
import :Vertex;

namespace ax /*::AxRender*/ {

template<u8 UV_COUNT, VertexNormalCount NORMAL_COUNT>
VertexLayout VertexLayout_make_color(Int colorCount) {
	switch (colorCount) {
		case 0: return Vertex_Simple<0, UV_COUNT, NORMAL_COUNT>::s_layout();
		case 1: return Vertex_Simple<1, UV_COUNT, NORMAL_COUNT>::s_layout();
		case 2: return Vertex_Simple<2, UV_COUNT, NORMAL_COUNT>::s_layout();
		case 3: return Vertex_Simple<3, UV_COUNT, NORMAL_COUNT>::s_layout();
		default: throw Error_Undefined(Fmt("VertexLayout::s_make unsupported color count {}", colorCount));
	}
}

template<VertexNormalCount NORMAL_COUNT>
VertexLayout VertexLayout_make_UV(Int colorCount, Int uvCount) {
	switch (uvCount) {
		case 0: return VertexLayout_make_color<0, NORMAL_COUNT>(colorCount);
		case 1: return VertexLayout_make_color<1, NORMAL_COUNT>(colorCount);
		case 2: return VertexLayout_make_color<2, NORMAL_COUNT>(colorCount);
		case 3: return VertexLayout_make_color<3, NORMAL_COUNT>(colorCount);
		case 4: return VertexLayout_make_color<4, NORMAL_COUNT>(colorCount);
		case 5: return VertexLayout_make_color<5, NORMAL_COUNT>(colorCount);
		case 6: return VertexLayout_make_color<6, NORMAL_COUNT>(colorCount);
		case 7: return VertexLayout_make_color<7, NORMAL_COUNT>(colorCount);
		default: throw Error_Undefined(Fmt("VertexLayout::s_make unsupported UV count {}", uvCount));
	}
}

VertexLayout VertexLayout_make_normal(Int colorCount, Int uvCount, VertexNormalCount normalCount) {
	using NormalCount = VertexNormalCount;
	switch (normalCount) {
		case NormalCount::None    : return VertexLayout_make_UV<NormalCount::None    >(colorCount, uvCount);
		case NormalCount::Normal  : return VertexLayout_make_UV<NormalCount::Normal  >(colorCount, uvCount);
		case NormalCount::Binormal: return VertexLayout_make_UV<NormalCount::Binormal>(colorCount, uvCount);
		case NormalCount::Tangent : return VertexLayout_make_UV<NormalCount::Tangent >(colorCount, uvCount);
		default: throw Error_Undefined(Fmt("VertexLayout::s_make unsupported normal count {}", normalCount));
	}
}

VertexLayout VertexLayout::s_make(Int colorCount, Int uvCount, NormalCount normalCount) {
	return VertexLayout_make_normal(colorCount, uvCount, normalCount);
}

VertexLayoutManager::VertexLayoutManager() {
}

VertexLayoutManager* VertexLayoutManager::s_instance() {
	static GlobalSingleton<VertexLayoutManager> s;
	return s.ptr();
}

VertexLayout VertexLayoutManager::registerLayout(const VertexLayoutDesc& desc) {
	auto md = _md.scopedLock();

	auto* node = md->table.findNode(desc);
	if (node) return node->value();

	auto& newNode = md->table.addNode(desc);
	newNode.value() = VertexLayout(&newNode.key()); // use Desc pointer from key

	return newNode.value();
}

}