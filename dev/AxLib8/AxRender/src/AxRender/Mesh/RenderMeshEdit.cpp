module AxRender;
import :RenderMeshEdit;
import :EditableMeshEdit;

namespace ax {

void RenderMeshEdit::addRect(VertexLayout vertexLayout, const Rect2f& rect, const Rect2f& uv, const Color4f& color_) {
// 0 ----- 1 
// |       |
// 2 ----- 3
	using Index = u16;
	auto indexType = VertexIndexType_get<Index>;
	auto primType  =  PrimType::Triangles;

	auto edit = _mesh.editNewVertices(primType, vertexLayout, indexType, 4);

	if (auto enumerator = edit.tryEditPosition()) {
		auto dst = enumerator->begin();
		dst->set(rect.xMin(), rect.yMin(), 0); ++dst;
		dst->set(rect.xMax(), rect.yMin(), 0); ++dst;
		dst->set(rect.xMin(), rect.yMax(), 0); ++dst;
		dst->set(rect.xMax(), rect.yMax(), 0); ++dst;
		if (dst != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditTexCoord0()) {
		auto dst = enumerator->begin();
		dst->set(uv.xMin(), uv.yMin()); ++dst;
		dst->set(uv.xMax(), uv.yMin()); ++dst;
		dst->set(uv.xMin(), uv.yMax()); ++dst;
		dst->set(uv.xMax(), uv.yMax()); ++dst;
		if (dst != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditColor0()) {
		auto color = Color4b::s_conv(color_.toPremultipliedAlpha());
		enumerator->fillValues(color);
	}

	auto  baseIdx = static_cast<Index>(edit.range.start());
	Index indices[] = {
		static_cast<Index>(baseIdx + 0),
		static_cast<Index>(baseIdx + 1),
		static_cast<Index>(baseIdx + 3),
		static_cast<Index>(baseIdx + 0),
		static_cast<Index>(baseIdx + 3),
		static_cast<Index>(baseIdx + 2)
	};

	edit.subMesh.indexBuffer.addIndices(Span<Index>(indices));
}

void RenderMeshEdit::createRect(VertexLayout vertexLayout, const Rect2f& rect, const Rect2f& uv, const Color4f& color) {
	_mesh.clear();
	addRect(vertexLayout, rect, uv, color);
}

void RenderMeshEdit::createBorderRect(VertexLayout vertexLayout, const Rect2f& rect, const Margin2f& border, const Rect2f& uv, const Margin2f& uvBorder, const Color4f& color, bool hasCenter) {
	_mesh.clear();
	addBorderRect(vertexLayout, rect, border, uv, uvBorder, color, hasCenter);
}

void RenderMeshEdit::addBorderRect(VertexLayout    vertexLayout,
                                   const Rect2f&   rect,
                                   const Margin2f& border,
                                   const Rect2f&   uv,
                                   const Margin2f& uvBorder,
                                   const Color4f&  color_,
                                   bool            hasCenter) {
//  0 --- 1 ----- 2 --- 3
//  |  \  |       |  /  |
//  4 --- 5 ----- 6 --- 7
//  |     |       |     |
//  8 --- 9 -----10 ---11
//  |  /  |       |  \ |
// 12 ---13 -----14 ---15

	using Index = u16;
	auto indexType = VertexIndexType_get<Index>;
	auto primType  =  PrimType::Triangles;

	auto  edit = _mesh.editNewVertices(primType, vertexLayout, indexType, 16);

	auto outerRect	= rect;
	auto innerRect	= rect - border;
	auto outerUv	= uv;
	auto innerUv	= uv - uvBorder;

	if (auto enumerator = edit.tryEditPosition()) {
		auto dst = enumerator->begin();
		// top row
		dst->set(outerRect.xMin(), outerRect.yMin(), 0); ++dst;
		dst->set(innerRect.xMin(), outerRect.yMin(), 0); ++dst;
		dst->set(innerRect.xMax(), outerRect.yMin(), 0); ++dst;
		dst->set(outerRect.xMax(), outerRect.yMin(), 0); ++dst;
		// 2nd row
		dst->set(outerRect.xMin(), innerRect.yMin(), 0); ++dst;
		dst->set(innerRect.xMin(), innerRect.yMin(), 0); ++dst;
		dst->set(innerRect.xMax(), innerRect.yMin(), 0); ++dst;
		dst->set(outerRect.xMax(), innerRect.yMin(), 0); ++dst;
		// 3rd row
		dst->set(outerRect.xMin(), innerRect.yMax(), 0); ++dst;
		dst->set(innerRect.xMin(), innerRect.yMax(), 0); ++dst;
		dst->set(innerRect.xMax(), innerRect.yMax(), 0); ++dst;
		dst->set(outerRect.xMax(), innerRect.yMax(), 0); ++dst;
		// bottom row
		dst->set(outerRect.xMin(), outerRect.yMax(), 0); ++dst;
		dst->set(innerRect.xMin(), outerRect.yMax(), 0); ++dst;
		dst->set(innerRect.xMax(), outerRect.yMax(), 0); ++dst;
		dst->set(outerRect.xMax(), outerRect.yMax(), 0); ++dst;
		if (dst != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditTexCoord0()) {
		auto dst = enumerator->begin();
		// top row
		dst->set(outerUv.xMin(), outerUv.yMin()); ++dst;
		dst->set(innerUv.xMin(), outerUv.yMin()); ++dst;
		dst->set(innerUv.xMax(), outerUv.yMin()); ++dst;
		dst->set(outerUv.xMax(), outerUv.yMin()); ++dst;
		// 2nd row
		dst->set(outerUv.xMin(), innerUv.yMin()); ++dst;
		dst->set(innerUv.xMin(), innerUv.yMin()); ++dst;
		dst->set(innerUv.xMax(), innerUv.yMin()); ++dst;
		dst->set(outerUv.xMax(), innerUv.yMin()); ++dst;
		// 3rd row
		dst->set(outerUv.xMin(), innerUv.yMax()); ++dst;
		dst->set(innerUv.xMin(), innerUv.yMax()); ++dst;
		dst->set(innerUv.xMax(), innerUv.yMax()); ++dst;
		dst->set(outerUv.xMax(), innerUv.yMax()); ++dst;
		// bottom row
		dst->set(outerUv.xMin(), outerUv.yMax()); ++dst;
		dst->set(innerUv.xMin(), outerUv.yMax()); ++dst;
		dst->set(innerUv.xMax(), outerUv.yMax()); ++dst;
		dst->set(outerUv.xMax(), outerUv.yMax()); ++dst;
		if (dst != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditColor0()) {
		auto color = Color4b::s_conv(color_);
		auto dst = enumerator->begin();
		// top row
		*dst = color; ++dst;
		*dst = color; ++dst;
		*dst = color; ++dst;
		*dst = color; ++dst;
		// 2nd row
		*dst = color; ++dst;
		*dst = color; ++dst;
		*dst = color; ++dst;
		*dst = color; ++dst;
		// 3rd row
		*dst = color; ++dst;
		*dst = color; ++dst;
		*dst = color; ++dst;
		*dst = color; ++dst;
		// bottom row
		*dst = color; ++dst;
		*dst = color; ++dst;
		*dst = color; ++dst;
		*dst = color; ++dst;
		if (dst != enumerator->end()) throw Error_Undefined();
	}

	Index indicesBuf[] = {
		4,0,5,5,0,1,      1,2,5,5,2,6,        2,3,6,6,3,7,
		4,5,8,8,5,9,                          6,7,10,10,7,11,
		8,9,12,12,9,13,   9,10,13,13,10,14,   14,10,15,15,10,11,
		5,6,9,9,6,10 //center
	};

	auto  baseIdx = static_cast<Index>(edit.range.start());
	auto indices = MutSpan(indicesBuf);
	for (auto& v : indices) {
		v += baseIdx;
	}	
	
	indices = hasCenter ? indices : indices.slice(0, 8 * 6);
	edit.addIndices(indices.constSpan());
}

void RenderMeshEdit::createCube(VertexLayout vertexLayout, const Vec3f& pos, const Vec3f& size, const Color4f& color_) {
//     4-------5
//    /|      /|
//   0-------1 |
//   | 6-----|-7
//   |/      |/
//   2-------3
	
	_mesh.clear();

	using Index = u16;
	auto edit = _mesh.editNewVertices(PrimType::Triangles, vertexLayout, VertexIndexType_get<Index>, 8);

	auto s = size * 0.5f;

	if (auto enumerator = edit.tryEditPosition()) {
		auto dst = enumerator->begin();
		{
			auto t = pos;
			t.z -= s.z;
			dst->set(t.x - s.x, t.y - s.y, t.z); ++dst;
			dst->set(t.x + s.x, t.y - s.y, t.z); ++dst;
			dst->set(t.x - s.x, t.y + s.y, t.z); ++dst;
			dst->set(t.x + s.x, t.y + s.y, t.z); ++dst;
		}

		{
			auto t = pos;
			t.z += s.z;
			dst->set(t.x - s.x, t.y - s.y, t.z); ++dst;
			dst->set(t.x + s.x, t.y - s.y, t.z); ++dst;
			dst->set(t.x - s.x, t.y + s.y, t.z); ++dst;
			dst->set(t.x + s.x, t.y + s.y, t.z); ++dst;
		}
		if (dst != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditColor0()) {
		auto color = Color4b::s_conv(color_);
		enumerator->fillValues(color);
	}

	Index indicesBuf[] = {
		0,1,3,0,3,2,
		1,5,7,1,7,3,
		4,5,1,4,1,0,
		5,4,6,5,6,7,
		4,0,2,4,2,6,
		2,3,7,2,7,6,
	};

	auto baseIdx = static_cast<Index>(edit.range.start());
	auto indices = MutSpan(indicesBuf);
	for (auto& v : indices) {
		v += baseIdx;
	}	
	
	edit.addIndices(Span<Index>(indices));
}

void RenderMeshEdit::createText(VertexLayout vertexLayout, StrView text, const Vec2f& pos, const FontStyle* style) {
	_mesh.clear();
	addText(vertexLayout, text, pos, style);
}

void RenderMeshEdit::addText(VertexLayout vertexLayout, StrView text, const Vec2f& pos, const FontStyle* style) {
	addTextBillboard(vertexLayout, text, pos.xy0(), style);
}

void RenderMeshEdit::createTextBillboard(VertexLayout vertexLayout, StrView text, const Vec3f& pos, const FontStyle* style) {
	_mesh.clear();
	addTextBillboard(vertexLayout, text, pos, style);
}

void RenderMeshEdit::addTextBillboard(VertexLayout vertexLayout, StrView text, const Vec3f& pos, const FontStyle* style) {
	if (!style) {
		style = RenderStockObjects::s_instance()->fonts->defaultFontStyle;
	}

	if (!style)			{ AX_ASSERT(false); return; }
	if (!style->font)	{ AX_ASSERT(false); return; }

	if (!vertexLayout->hasPosition()) { AX_ASSERT(false); return; }

	Array<FontGlyph, 128> glyphs;
	style->font->getGlyphs(glyphs, text);

	auto glyphCount  = glyphs.size();
	if (!glyphCount) return;

	auto vertexCount = glyphCount * 4;
	auto indexCount  = glyphCount * 6;

	using Index = u16;
	auto primType  =  PrimType::Triangles;

	auto edit = _mesh.editNewVertices(primType, vertexLayout, VertexIndexType_get<Index>, vertexCount);
	auto baseIdx = static_cast<Index>(edit.range.start());

	auto textColor = Color4b::s_conv(style->color);

// 0 ----- 1
// |       |
// 2 ----- 3

	auto posEnumerator = edit.tryEditPosition();
	auto dstIndices = edit.editNewIndices<Index>(indexCount);
	auto dstIndex = dstIndices.begin();

	struct Pen {
		float maxHeightInThisLine = 0;
		Vec2f pos{0,0};
		void advance(FontGlyph& g) {
			Math::max_itself(maxHeightInThisLine, g.outerRect.h);
			pos.x += g.outerRect.w + g.kerning.x;
			if (g.charCode == '\n') {
				pos.y += Math::ceil(maxHeightInThisLine * 1.08f);
				maxHeightInThisLine = 0;
				pos.x = 0;
			}
		}
	};

	{
		Pen pen;
		auto dstPos = posEnumerator->begin();
		for (auto& g : glyphs) {
			auto rc = g.vertexRect + pen.pos;

			*dstPos = pos + rc.xMin_yMin().xy0(); ++dstPos;
			*dstPos = pos + rc.xMax_yMin().xy0(); ++dstPos;
			*dstPos = pos + rc.xMin_yMax().xy0(); ++dstPos;
			*dstPos = pos + rc.xMax_yMax().xy0(); ++dstPos;

			*dstIndex = static_cast<Index>(baseIdx + 0); ++dstIndex;
			*dstIndex = static_cast<Index>(baseIdx + 1); ++dstIndex;
			*dstIndex = static_cast<Index>(baseIdx + 3); ++dstIndex;
			*dstIndex = static_cast<Index>(baseIdx + 0); ++dstIndex;
			*dstIndex = static_cast<Index>(baseIdx + 3); ++dstIndex;
			*dstIndex = static_cast<Index>(baseIdx + 2); ++dstIndex;

			pen.advance(g);
			baseIdx += 4;
		}
		if (dstPos != posEnumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditTexCoord0()) {
		auto dstUv0 = enumerator->begin();
		for (auto& g : glyphs) {
			*dstUv0 = g.uv.xMin_yMin(); ++dstUv0;
			*dstUv0 = g.uv.xMax_yMin(); ++dstUv0;
			*dstUv0 = g.uv.xMin_yMax(); ++dstUv0;
			*dstUv0 = g.uv.xMax_yMax(); ++dstUv0;
		}
		if (dstUv0 != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditTexCoord1()) {
		auto dstUv1 = enumerator->begin();
		Pen pen;
		for (auto& g : glyphs) {
			auto rc = g.vertexRect + pen.pos;
			*dstUv1 = rc.xMin_yMin(); ++dstUv1;
			*dstUv1 = rc.xMax_yMin(); ++dstUv1;
			*dstUv1 = rc.xMin_yMax(); ++dstUv1;
			*dstUv1 = rc.xMax_yMax(); ++dstUv1;

			pen.advance(g);
		}
		if (dstUv1 != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditColor0()) {
		enumerator->fillValues(textColor);
	}
}

void RenderMeshEdit::createGrid(RenderPlaneAxis planeAxis,
                                VertexLayout    vertexLayout,
                                float           cellSize,
                                Int             cellCount,
                                const Color4f&  centerLineColor_,
                                const Color4f&  gridLineColor_,
                                const Color4f&  gridLine2_Color_,
                                Int             gridLine2_Interval
) {
	_mesh.clear();
	
	Vec3f axis0;
	Vec3f axis1;
	
	switch (planeAxis) {
		case RenderPlaneAxis::XY: axis0.set(1,0,0); axis1.set(0,1,0); break;
		case RenderPlaneAxis::YZ: axis0.set(0,1,0); axis1.set(0,0,1); break;
		case RenderPlaneAxis::ZX: axis0.set(0,0,1); axis1.set(1,0,0); break;
	}

	const Int dx = cellCount / 2;

	const float scale = cellSize * static_cast<f32>(dx);

	const Int dx1 = dx + 1;

	auto vertexCount = (dx + dx) * 4 + 4;

	_mesh.clear();
	_mesh.setSubMeshCount(1);

	auto edit = _mesh.editNewVertices(PrimType::Lines, vertexLayout, VertexIndexType::None, vertexCount);

	auto posEnumerator = edit.tryEditPosition();
	auto colEnumerator = edit.tryEditColor0();
	if (!posEnumerator) throw Error_Undefined();
	
	auto dstPos = posEnumerator->begin();
	auto dstCol = colEnumerator ? colEnumerator->begin() : ElemIter<Color4b>();

	//center line X
	*dstPos = -axis0 * scale; ++dstPos;
	*dstPos =  axis0 * scale; ++dstPos;
	
	//center line Y
	*dstPos = -axis1 * scale; ++dstPos;
	*dstPos =  axis1 * scale; ++dstPos;

	if (dstCol) {
		{
			auto centerLineColor = Color4b::s_conv(centerLineColor_ * Color4f(0,0,1,1));
			*dstCol = centerLineColor; ++dstCol;
			*dstCol = centerLineColor; ++dstCol;
		}
		{
			auto centerLineColor = Color4b::s_conv(centerLineColor_ * Color4f(1,0,0,1));
			*dstCol = centerLineColor; ++dstCol;
			*dstCol = centerLineColor; ++dstCol;
		}
	}
	
	for (Int x = 1; x < dx1; x++) {
		float px = static_cast<f32>(x) / static_cast<f32>(dx);
		// line X
		*dstPos = (axis0 *  px + axis1 * -1) * scale; ++dstPos;
		*dstPos = (axis0 *  px + axis1 *  1) * scale; ++dstPos;
		*dstPos = (axis0 * -px + axis1 * -1) * scale; ++dstPos;
		*dstPos = (axis0 * -px + axis1 *  1) * scale; ++dstPos;

		// line Y
		*dstPos = (axis0 * -1 + axis1 *  px) * scale; ++dstPos;
		*dstPos = (axis0 *  1 + axis1 *  px) * scale; ++dstPos;
		*dstPos = (axis0 * -1 + axis1 * -px) * scale; ++dstPos;
		*dstPos = (axis0 *  1 + axis1 * -px) * scale; ++dstPos;

		if (dstCol) {
			auto gridLine2_Color = Color4b::s_conv(gridLine2_Color_);
			auto gridLineColor   = Color4b::s_conv(gridLineColor_);
			auto color = (gridLine2_Interval != 0 && x % gridLine2_Interval == 0) ? gridLine2_Color : gridLineColor;
			for (Int j = 0; j < 8; ++j) {
				*dstCol = color; ++dstCol;
			}
		}
	}

	if (dstPos != posEnumerator->end()) throw Error_Undefined();
	if (dstCol) {
		if (dstCol != colEnumerator->end()) throw Error_Undefined();
	}
}

void RenderMeshEdit::createAxis(VertexLayout vertexLayout) {
	auto edit = _mesh.editNewVertices(PrimType::Lines, vertexLayout, VertexIndexType::None, 6);
	
	if (auto enumerator = edit.tryEditPosition()) {
		auto dstPos = enumerator->begin();
		*dstPos = Vec3f(0,0,0); ++dstPos;
		*dstPos = Vec3f(1,0,0); ++dstPos;
		
		*dstPos = Vec3f(0,0,0); ++dstPos;
		*dstPos = Vec3f(0,1,0); ++dstPos;
		
		*dstPos = Vec3f(0,0,0); ++dstPos;
		*dstPos = Vec3f(0,0,1); ++dstPos;
		
		if (dstPos != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditColor0()) {
		auto dstCol = enumerator->begin();
		*dstCol = Color4b::kRed();   ++dstCol;
		*dstCol = Color4b::kRed();   ++dstCol;

		*dstCol = Color4b::kGreen(); ++dstCol;
		*dstCol = Color4b::kGreen(); ++dstCol;

		*dstCol = Color4b::kBlue();  ++dstCol;
		*dstCol = Color4b::kBlue();  ++dstCol;
		
		if (dstCol != enumerator->end()) throw Error_Undefined(); 
	}
}

void RenderMeshEdit::createLines(VertexLayout vertexLayout, Span<Vec3f> positions, const Color4f& color_) {
	_mesh.clear();

	Int n = positions.size();
	if (n < 2) return;

	AX_ASSERT(n % 2 == 0);

	auto edit = _mesh.editNewVertices(PrimType::Lines, vertexLayout, VertexIndexType::None, n);

	if (auto enumerator = edit.tryEditPosition()) {
		auto dst = enumerator->begin();
		for (auto& src : positions) {
			*dst = src;
			++dst;
		}
		if (dst != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditColor0()) {
		auto color = Color4b::s_conv(color_);
		enumerator->fillValues(color);
	}
}

void RenderMeshEdit::createLines(VertexLayout vertexLayout, Span<Vec2f> positions, const Color4f& color_) {
	_mesh.clear();

	Int n = positions.size();
	if (n < 2) return;

	AX_ASSERT(n % 2 == 0);
	auto edit = _mesh.editNewVertices(PrimType::Lines, vertexLayout, VertexIndexType::None, n);

	if (auto enumerator = edit.tryEditPosition()) {
		auto dst = enumerator->begin();
		for (auto& src : positions) {
			*dst = src.xy0();
			++dst;
		}
		if (dst != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditColor0()) {
		auto color = Color4b::s_conv(color_);
		enumerator->fillValues(color);
	}
}

void RenderMeshEdit::createLineStrip(VertexLayout vertexLayout, Span<Vec3f> positions, const Color4f& color_) {
	_mesh.clear();

	const Int n = positions.size();
	if (n < 2) return;

	const Int segmentCount = n - 1;
	auto edit = _mesh.editNewVertices(PrimType::Lines, vertexLayout, VertexIndexType::None, segmentCount * 2);

	if (auto enumerator = edit.tryEditPosition()) {
		auto dst = enumerator->begin();

		for (Int i = 0; i < segmentCount; i++) {
			*dst = positions[i];
			++dst;

			*dst = positions[i+1];
			++dst;
		}
		
		if (dst != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditColor0()) {
		auto color = Color4b::s_conv(color_);
		enumerator->fillValues(color);
	}
}

void RenderMeshEdit::createLineStrip(VertexLayout vertexLayout, Span<Vec2f> positions, const Color4f& color_) {
	_mesh.clear();

	Int n = positions.size();
	if (n < 2) return;

	Int segmentCount = n - 1;
	auto edit = _mesh.editNewVertices(RenderPrimitiveType::Lines, vertexLayout, VertexIndexType::None, segmentCount * 2);

	if (auto enumerator = edit.tryEditPosition()) {
		auto dst = enumerator->begin();
		for (Int i = 0; i < segmentCount; i++) {
			*dst = positions[i].xy0();
			++dst;

			*dst = positions[i+1].xy0();
			++dst;
		}
		if (dst != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditColor0()) {
		auto color = Color4b::s_conv(color_);
		enumerator->fillValues(color);
	}
}

void RenderMeshEdit::createFromEditableMesh(VertexLayout vertexLayout, EditableMesh& srcMesh) {
	_mesh.clear();
	addFromEditableMesh(vertexLayout, srcMesh);
}

void RenderMeshEdit::addFromEditableMesh(VertexLayout vertexLayout, EditableMesh& srcMesh) {
	Array<EditableMesh::Point*, 64> faceVertices;

	using Index = u16;
	auto indexType = VertexIndexType_get<Index>;
	auto primType = PrimType::Triangles;

	//TODO polygon triangulate
	for (auto& face : srcMesh.faces()) {
		const auto fvCount = face.pointCount();
		auto edit = _mesh.editNewVertices(primType, vertexLayout, indexType, fvCount);

		if (auto enumerator = edit.tryEditPosition()) {
			face.getPoints(srcMesh, faceVertices);

			auto dst = enumerator->begin();
			for (auto& fv : faceVertices) {
				*dst = Vec3f::s_cast(fv->pos);
				++dst;
			}
			if (dst != enumerator->end()) throw Error_Undefined();
		}

		if (auto enumerator = edit.tryEditNormal0()) {
			auto dst = enumerator->begin();
			auto src = face.getNormals(srcMesh);
			for (auto& p : src) {
				*dst = Vec3f::s_cast(p);
				++dst;
			}
			if (dst != enumerator->end()) throw Error_Undefined();
		}

		// color set
		for (Int i = 0; i < srcMesh.colorSetCount(); i++) {
			if (auto enumerator = edit.tryEditColor(i)) {
				auto dst = enumerator->begin();
				auto src = face.getColors(srcMesh, i);
				for (auto& p : src) {
					*dst = Color4b::s_conv(p); 
					++dst;
				}
				if (dst != enumerator->end()) throw Error_Undefined();
			}
		}

		// uv set
		for (Int i = 0; i < srcMesh.uvSetCount(); i++) {
			if (auto enumerator = edit.tryEditTexCoord(i)) {
				auto dst = enumerator->begin();
				auto src = face.getUvs(srcMesh, i);
				for (auto& p : src) {
					*dst = Vec2f::s_cast(p);
					++dst;
				}
				if (dst != enumerator->end()) throw Error_Undefined();
			}
		}

		//add vertex indices
		if (fvCount >= 3) {
			auto triCount = fvCount - 2;
			auto startVi = static_cast<Index>(edit.range.start());
			auto indices = edit.editNewIndices<Index>(triCount * 3);
			auto vi = indices.begin();
			for (Int tri = 0; tri < triCount; tri++) {
				*vi = static_cast<Index>(startVi          ); ++vi;
				*vi = static_cast<Index>(startVi + tri + 1); ++vi;
				*vi = static_cast<Index>(startVi + tri + 2); ++vi;
			}
			if (vi != indices.end()) throw Error_Undefined();
		}
	}
}

void RenderMeshEdit::createSphere(VertexLayout vertexLayout, float radius, Int rows, Int columns) {
	EditableMesh tmp;
	EditableMeshEdit(tmp).createSphere(radius, rows, columns);
	tmp.addColorSet(Color4f(1,1,1,1));
	createFromEditableMesh(vertexLayout, tmp);
}

void RenderMeshEdit::createCylinder(VertexLayout vertexLayout, float height, float radius, Int rows, Int columns, bool topCap, bool bottomCap) {
	EditableMesh tmp;
	EditableMeshEdit(tmp).createCylinder(height, radius, rows, columns, topCap, bottomCap);
	tmp.addColorSet(Color4f(1,1,1,1));
	createFromEditableMesh(vertexLayout, tmp);
}

void RenderMeshEdit::createCone(VertexLayout vertexLayout, float height, float radius, Int rows, Int columns, bool bottomCap) {
	EditableMesh tmp;
	EditableMeshEdit(tmp).createCone(height, radius, rows, columns, bottomCap);
	tmp.addColorSet(Color4f(1,1,1,1));
	createFromEditableMesh(vertexLayout, tmp);
}

void RenderMeshEdit::setColor(const Color4f& color_, Int colorSet) {
	for (auto& sm : _mesh.subMeshes()) {
		auto sem = VertexSemantic::COLOR0 + static_cast<u16>(colorSet);
		if (auto enumerator = sm.vertexBuffer.tryEditElements<Color4b>(sem)) {
			auto color = Color4b::s_conv(color_);
			enumerator->fillValues(color);
		}
	}
}

} // namespace
