module;

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include FT_OUTLINE_H
#include FT_SIZES_H

export module AxRender:Font_FreeType;
export import :Font;

export namespace ax {

class FontFile_FreeType;
class FontProvider_FreeType;

class FontFace_FreeType : public FontFace {
public:
	using FontFile = FontFile_FreeType;

	FontFace_FreeType(FontFile* fontFile, InNameId userDefineName);
	~FontFace_FreeType();

	SPtr<Font> createFont(const Font_CreateDesc& desc);

	Mutex _mutex;

	SPtr<FontFile> _fontFile;
	FT_Face _face {nullptr};
};


class FontFile_FreeType : public RenderObject {
	AX_RTTI_INFO(FontProvider_FreeType, RenderObject)
public:
	FontFile_FreeType(FontProvider_FreeType* provider);

	SPtr<FontFace> loadFile	(StrView filename,				InNameId userDefineName);
	SPtr<FontFace> loadMem	(ByteSpan mem, bool copyMem,	InNameId userDefineName);
	SPtr<FontFace> loadMem	(IByteArray && mem,				InNameId userDefineName);

	SPtr<Font> createFont(const Font_CreateDesc& desc);

	SPtr<FontFace> _doLoad	(InNameId userDefineName);

	FontProvider_FreeType* _provider {nullptr};
	FileMemMap		_mapping;
	ByteArray		_memBuf;
	ByteSpan		_memSpan;
};


class FontProvider_FreeType : public FontProvider {
	AX_RTTI_INFO(FontProvider_FreeType, FontProvider);
public:
	using FontFile	 = FontFile_FreeType;
	using FontFace	 = FontFace;
	using FontFaceFT = FontFace_FreeType;

	FontProvider_FreeType();
	~FontProvider_FreeType();
		
	virtual FontFace* loadFontFile	(StrView filename,				StrView userDefineName) override;
	virtual FontFace* loadFontMem	(ByteSpan mem, bool copyMem,	StrView userDefineName) override;
	virtual FontFace* loadFontMem	(IByteArray && mem,			StrView userDefineName) override;

	virtual SPtr<Font> onCreateFont(const Font_CreateDesc& desc) override;

	void onNewFile(FontFile* f);
	void onNewFace(FontFaceFT* f);
	void onNewFont(Font* f);

	FT_Library		lib() { return _lib; }

private:
	FT_MemoryRec_	_allocatorWrapper;
	FT_Library		_lib{nullptr};

	Array<SPtr<FontFile>>	_fontFiles;
	Array<SPtr<FontFaceFT>>	_fontFaces;
	Array<SPtr<Font>>		_fonts;
};

class Font_FreeType : public Font {
	AX_RTTI_INFO(Font_FreeType, Font);
public:
	~Font_FreeType();

	Font_FreeType(const Desc& desc, FontFace_FreeType* face);

	virtual void onGetGlyphs(MutSpan<FontGlyph> outGlyphs, StrViewU text) override;

private:
	FontGlyph* _addGlyph(CharU ch);
	SPtr<FontFace_FreeType> _fontFace;
	FT_Size _fontWithSize{nullptr};
};

}// namespace