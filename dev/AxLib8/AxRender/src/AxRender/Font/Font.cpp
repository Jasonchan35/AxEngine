module AxRender;
import :RenderSystem_Backend;
import :Font;
import :Font_FreeType;
import :Texture_Backend;

namespace ax {

Font::Font(const Font_CreateDesc& desc) {
	_desc = desc;
	_hasKerning = false;
}

void Font::getGlyphs(IArray<FontGlyph>& outGlyphs, StrView text) const {
	getGlyphs(outGlyphs, TempStringU::s_utf(text));
}

void Font::getGlyphs(IArray<FontGlyph>& outGlyphs, StrViewU text) const {
	outGlyphs.clear();
	outGlyphs.resize(text.size());
	ax_const_cast(this)->onGetGlyphs(outGlyphs.span(), text);
}

#if 0
#pragma mark ----------- FontProvider ------------
#endif

SPtr<Font> FontProvider::createFont(const Font_CreateDesc& desc) {
	return onCreateFont(desc);
}

#if 0
#pragma mark ----------- FontGlyphCache ------------
#endif

/*
http://clb.demon.fi/files/RectangleBinPack.pdf
Two-Dimensional Rectangle Bin Packing
- The Skyline Algorithms

*/

void FontGlyphCache::create(Vec2i size) {
	_packer.create(size);
	_image.create(size);

	Texture2D_CreateDesc desc;
	desc.imageInfo = _image.info(); 
	_tex = SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newTexture2D(AX_NEW, desc));
}

void FontGlyphCache::onDebugRender(RenderRequest& req) {
#if 0
	if (!_tex) return;
	Vec2f drawPos(0, 200);
	req.ui.drawTexture(AX_LOC, _tex, drawPos);

	{
		Array<Vec2f, 256> lines;

		auto skyline = _packer.skyline();
		auto n  = skyline.size();
		if (n < 2)
			return;

		auto* s = skyline.data();
		for (Int i = 1; i < n; i++, s++) {
			lines.append(drawPos + Vec2f::s_cast(Vec2i(s[0].x, s[0].y)));
			lines.append(drawPos + Vec2f::s_cast(Vec2i(s[1].x, s[0].y)));
		}
		req.ui.drawLineStrip(AX_LOC, lines, Color4f(0,1,0, 0.5f));
	}
#endif
}

bool FontGlyphCache::onAddGlyph(Font& font, FontGlyph& glyph, const ImageInfo& info, ByteSpan pixelData) {
	const Int padding = 1;

	auto result = _packer.addRect(info.size + Vec2i::s_all(padding * 2));
	if (result.rect.x == 0) return false;

	auto uv = Rect2f::s_cast(result.rect);
	uv.x += padding;
	uv.y += padding;
	uv.w -= padding * 2;
	uv.h -= padding * 2;
	uv /= Vec2f::s_cast(_image.size());

	glyph.uv = uv;

	_image.subImage(result.rect.pos + Vec2i::s_all(padding), info, pixelData);
	_offset.x += info.size.x + 1;
	return true;
}

#if 0
#pragma mark ----------- FontManager ------------
#endif

FontManager* FontManager_instance;

FontManager* FontManager::s_instance() {
	return FontManager_instance;
}

FontManager::FontManager() {
	AX_ASSERT(FontManager_instance == nullptr);

	FontManager_instance = this;
	_glyphCache.create(Vec2i(1024, 1024));
	_provider = SPtr_new<FontProvider_FreeType>(AX_NEW);
}

FontManager::~FontManager() {
}

FontFace* FontManager::loadFontFile(StrView filename, StrView userDefineName) {
	return _provider->loadFontFile(filename, userDefineName);
}

SPtr<Font> FontManager::createFont(const Font_CreateDesc& desc) {
	return _provider->createFont(desc);
}

SPtr<Font> FontManager::createFont(InNameId face, Int size, bool bold, bool italic) {
	Font_CreateDesc desc;
	desc.fontName = face;
	desc.size     = size;
	desc.bold     = bold;
	desc.italic   = italic;
	return createFont(desc);
}

void FontManager::onDebugRender(RenderRequest& req) {
	_glyphCache.onDebugRender(req);
}



} // namespace