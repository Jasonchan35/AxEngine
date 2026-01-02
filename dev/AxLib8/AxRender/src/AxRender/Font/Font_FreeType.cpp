module;

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include FT_OUTLINE_H
#include FT_SIZES_H

module AxRender;
import :Font_FreeType;

namespace ax {

#if 0
#pragma mark ------ Font_FreeType ----------------
#endif

Font_FreeType::~Font_FreeType() {
	if (!_fontFace) return;
	auto lock = _fontFace->_mutex.scopedLock(); 
	if (_fontWithSize) {
		FT_Done_Size(_fontWithSize);
		_fontWithSize = nullptr;
	}
}

Font_FreeType::Font_FreeType(const Desc& desc, FontFace_FreeType* fontFace) 
	: Base(desc)
{
	_fontFace.ref(fontFace);

	auto lock = _fontFace->_mutex.scopedLock();
	auto face = fontFace->_face;

	FT_Error err;
	err = FT_New_Size(face, &_fontWithSize);
	if (err) throw Error_Undefined();

	err = FT_Activate_Size(_fontWithSize);
	if (err) throw Error_Undefined();

	//auto pts = 64;
	//auto dpi = 300;
	//err = FT_Set_Char_Size(face, 0, 16 * pts, dpi, dpi);
	//if (err) throw Error_Undefined();
		
	err = FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(desc.size));
	if (err) throw Error_Undefined();

	_lineHeight = static_cast<f32>(_fontWithSize->metrics.height) / 64.0f;

	Rect2f rc(0, 0, static_cast<f32>(_fontWithSize->metrics.max_advance) / 64.0f, _lineHeight);
	_missingGlyph.outerRect  = rc;
	_missingGlyph.vertexRect = rc;

	_hasKerning = FT_HAS_KERNING(_fontFace->_face) != 0;
}

void Font_FreeType::onGetGlyphs(MutSpan<FontGlyph> outGlyphs, StrViewU text) {
	if (outGlyphs.size() != text.size()) throw Error_Undefined();
	auto lock = _fontFace->_mutex.scopedLock();
	
	FT_Error err;
	err = FT_Activate_Size(_fontWithSize);
	if (err) throw Error_Undefined();

	auto* face = _fontFace->_face;
	err = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	if (err) throw Error_Undefined();

	Int lastGlyphIndex = 0;
	auto* ch = text.begin();

	bool hasKerning = FT_HAS_KERNING(face);

	for (auto& dst : outGlyphs) {
		auto* src = _cachedGlyphs.find(*ch);
		if (!src) {
			src = _addGlyph(*ch);
		}
		if (!src) {
			//throw Error_Undefined();
			return;
		}

		dst = *src;

		if (!hasKerning) {
			dst.kerning.set(0, 0);

		} else if (lastGlyphIndex) {
			FT_Vector  kerning;
			FT_Get_Kerning(face, static_cast<FT_UInt>(lastGlyphIndex), static_cast<FT_UInt>(src->glyphIndex), FT_KERNING_DEFAULT, &kerning);
			dst.kerning = Vec2f(static_cast<float>(kerning.x), static_cast<float>(kerning.y)) / 64.0f;
		}

		lastGlyphIndex = src->glyphIndex;
		ch++;
	}
	AX_ASSERT(ch == text.end());
}

FontGlyph* Font_FreeType::_addGlyph(CharU ch) {
	auto lock = _fontFace->_mutex.scopedLock();

	auto glyph = _missingGlyph;
	auto* face = _fontFace->_face;

	glyph.charCode   = ch;

	if (ch == '\n' || ch == '\t') {
		glyph.vertexRect.set(0,0,0,0);
	} else {
		auto glyphIndex = FT_Get_Char_Index(face, ch);
		if (!glyphIndex) {
			return &_missingGlyph;
		}

		glyph.glyphIndex	= glyphIndex;

		FT_Error err;
		err = FT_Load_Glyph(face, glyphIndex, FT_LOAD_FORCE_AUTOHINT);
		if (err) throw Error_Undefined();

		FT_GlyphSlot  slot = face->glyph;

		err = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL); /* render mode */
		if (err) throw Error_Undefined();

		if (slot->bitmap.pixel_mode != ax_enum_int(FT_PIXEL_MODE_GRAY)) {
			throw Error_Undefined();
		}

		auto& bmp = slot->bitmap;
		auto* pixels	= reinterpret_cast<ColorRb*>(bmp.buffer);

		ImageInfo info;
		info.size.set(ax_safe_cast_from(bmp.width), ax_safe_cast_from(bmp.rows));
		info.strideInBytes = bmp.pitch;
		info.colorType = ColorType::Rb;
		
		if (bmp.rows == 0) {
			pixels = nullptr;
			info.strideInBytes = 0;
		}else if (info.strideInBytes < 0) { //upside down
			pixels = pixels + (bmp.rows-1) * info.strideInBytes;
			info.strideInBytes = -info.strideInBytes;
		}

		f32 ascenter = static_cast<f32>(_fontWithSize->metrics.ascender) / 64.0f;

		glyph.vertexRect.set(	static_cast<f32>(slot->bitmap_left) / 64.0f, 
								ascenter - static_cast<f32>(slot->bitmap_top),
								static_cast<f32>(bmp.width),
								static_cast<f32>(bmp.rows));

		glyph.outerRect.set(0, 0, static_cast<f32>(slot->advance.x) / 64.0f, _lineHeight);

		MutByteSpan pixelData(reinterpret_cast<Byte*>(pixels), bmp.rows * info.strideInBytes);
		if (!FontManager::s_instance()->glypchCache()->onAddGlyph(*this, glyph, info, pixelData)) {
			return nullptr;
		}
	}

	auto& o = _cachedGlyphs.add(ch, glyph);
	return &o;
}

/*
void Font_FreeType::onGetKerning(axMutSpan<LayoutGlyph> view) {
	if (view.size() <= 0) return;

	auto lock = ax_lock(_fontFace->_mutex);

	auto err = FT_Activate_Size(_fontWithSize);
	if (err) throw Error_Undefined();

	auto* dst = view.data();
	auto* end = dst + view.size();

	dst->kerning.set(0, 0);

	axInt prevGlyphIndex = 0;
	axInt curGlyphIndex = 0;
	if (auto* c = _cached.find(dst->character)) {
		prevGlyphIndex = curGlyphIndex = c->glyphIndex;
	}
	dst++;

	auto face = _fontFace->_face;

	FT_Vector  delta;
	for (; dst < end; dst++) {

		if (auto* c = _cached.find(dst->character)) {
			curGlyphIndex = c->glyphIndex;
		}

		FT_Get_Kerning(face, prevGlyphIndex, curGlyphIndex, FT_KERNING_DEFAULT, &delta);
		dst->kerning = axVec2f(delta.x, delta.y) / 64.0;

		prevGlyphIndex = curGlyphIndex;
	}	
}
*/

#if 0
#pragma mark ------ FontFace_FreeType ----------------
#endif

FontFace_FreeType::FontFace_FreeType(FontFile* fontFile, InNameId userDefineName) {
	auto lock = _mutex.scopedLock();
	
	_fontFile.ref(fontFile);
	auto* provider = _fontFile->_provider;
	auto& memSpan = _fontFile->_memSpan;

	auto err = FT_New_Memory_Face(provider->lib(), memSpan.data(), static_cast<FT_Long>(memSpan.size()), 0, &_face);
	if (err) {
		throw Error_Undefined();
	}

	_family     = NameId::s_make(StrView_c_str(_face->family_name));
	_styleName  = NameId::s_make(StrView_c_str(_face->style_name));

	if (_styleName == AX_NAMEID("Regular")) {
		_styleName = NameId();
	}

	if (userDefineName) {
		_name = userDefineName;
	}else if (_styleName) {
		_name = FmtName("{}-{}", _family, _styleName);
	} else {
		_name = _family;
	}

	provider->onNewFace(this);
}

FontFace_FreeType::~FontFace_FreeType() {
	auto lock = _mutex.scopedLock();
	if (_face) {
		FT_Done_Face(_face);
		_face = nullptr;
	}
}

SPtr<Font> FontFace_FreeType::createFont(const Font_CreateDesc& desc) {
	auto lock = _mutex.scopedLock();
	if (!_face) {
		throw Error_Undefined();
	}
	auto outFont = SPtr_new<Font_FreeType>(AX_NEW, desc, this);
	_fontFile->_provider->onNewFont(outFont);
	return outFont.ptr();
}

#if 0
#pragma mark ------ FontFile_FreeType ----------------
#endif

FontFile_FreeType::FontFile_FreeType(FontProvider_FreeType* provider) 
	: _provider(provider)
{
	provider->onNewFile(this);
}

SPtr<FontFace> FontFile_FreeType::loadFile(StrView filename, InNameId userDefineName) {
	_mapping.openFile(filename);
	return loadMem(_mapping, false, userDefineName);
}

SPtr<FontFace> FontFile_FreeType::loadMem(ByteSpan mem, bool copyMem, InNameId userDefineName) {
	if (copyMem) {
		_memBuf  = mem;
		_memSpan = _memBuf;
	}else{
		_memSpan = mem;
	}
	return _doLoad(userDefineName);
}

SPtr<FontFace> FontFile_FreeType::loadMem(IByteArray && mem, InNameId userDefineName) {
	_memBuf  = std::move(mem);
	_memSpan = _memBuf;
	return _doLoad(userDefineName);
}

SPtr<FontFace> FontFile_FreeType::_doLoad(InNameId userDefineName) {
	return SPtr_new<FontFace_FreeType>(AX_NEW, userDefineName);
}

#if 0
#pragma mark ------ FontProvider_FreeType ----------------
#endif

void* FontProvider_FreeType_MemAlloc(FT_Memory memory, long  size) {
	if (size <= 0) {
		AX_ASSERT(false);
		return nullptr;
	}
	void* p = ::malloc((size_t)size);
	if (p) {
		::memset(p, 0, (size_t)size);
	}
	return p;
}

void* axFontProvider_FreeType_MemRealloc(FT_Memory memory, long  cur_size, long new_size, void* block) {
	if (new_size <=0 ) {
		AX_ASSERT(false);
		return nullptr;
	}
	return ::realloc(block, (size_t)new_size);
}

void FontProvider_FreeType_MemFree(FT_Memory memory, void* block) {
	::free(block);
}

FontProvider_FreeType::FontProvider_FreeType() {
	_allocatorWrapper.user		= ax_default_allocator();
	_allocatorWrapper.alloc		= &FontProvider_FreeType_MemAlloc;
	_allocatorWrapper.realloc	= &axFontProvider_FreeType_MemRealloc;
	_allocatorWrapper.free		= &FontProvider_FreeType_MemFree;

	auto error = FT_New_Library(&_allocatorWrapper, &_lib);
	if ( error ) {
		throw Error_Undefined();
	}		

	FT_Add_Default_Modules(_lib);
}

FontProvider_FreeType::~FontProvider_FreeType() {
	for (auto& f : _fonts) {
		if (f->_getSPtrRefCount() != 1) {
			AX_ASSERT(false);
		}
	}
	_fonts.clear();
//-----
	for (auto& f : _fontFaces) {
		if (f->_getSPtrRefCount() != 1) {
			AX_ASSERT(false);
		}
	}
	_fontFaces.clear();

//-----
	for (auto& f : _fontFiles) {
		if (f->_getSPtrRefCount() != 1) {
			AX_ASSERT(false);
		}
	}
	_fontFiles.clear();

//-----
	if (_lib) {
		FT_Done_Library(_lib);
		_lib = nullptr;
	}
}

FontFace* FontProvider_FreeType::loadFontFile(StrView filename, StrView userDefineName) {
	auto o = SPtr_new<FontFile_FreeType>(AX_NEW, this);
	return o->loadFile(filename, userDefineName);
}

FontFace* FontProvider_FreeType::loadFontMem(ByteSpan mem, bool copyMem, StrView userDefineName) {
	auto o = SPtr_new<FontFile_FreeType>(AX_NEW, this);
	return o->loadMem(mem, copyMem, userDefineName);
}

FontFace* FontProvider_FreeType::loadFontMem(IByteArray && mem, StrView userDefineName) {
	auto o = SPtr_new<FontFile_FreeType>(AX_NEW, this);
	return o->loadMem(std::move(mem), userDefineName);
}

SPtr<Font> FontProvider_FreeType::onCreateFont(const Font_CreateDesc& desc) {
	for (auto& f : _fontFaces) {
		if (f->name() == desc.fontName) {
			return f->createFont(desc);
		}
	}
	throw Error_Undefined();
}

void FontProvider_FreeType::onNewFile(FontFile* f) {
	_fontFiles.emplaceBack(f);
}

void FontProvider_FreeType::onNewFont(Font* f) {
	_fonts.emplaceBack(f);
}

void FontProvider_FreeType::onNewFace(FontFaceFT* f) {
	_fontFaces.emplaceBack(f);
}

} // namespace