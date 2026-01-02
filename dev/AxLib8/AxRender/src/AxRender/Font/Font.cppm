module;

export module AxRender:Font;
export import :RenderDataType;
export import :RectBinPacker;
export import :Image;

export namespace ax {

class FontGlyphLayout;
class FontProvider;
class FontFace;
class Font;

class Font_CreateDesc {
public:
	NameId		fontName;
	Int			size = 0;
	bool		bold   : 1 = false;
	bool		italic : 1 = false;
};

class FontGlyph {
public:
	CharU	charCode = 0;
	Int		glyphIndex = 0;
	Rect2f	outerRect	{0,0,0,0};
	Rect2f	vertexRect	{0,0,0,0};
	Rect2f	uv			{0,0,0,0};
	Vec2f	kerning		{0,0};
};

class Font : public RenderObject {
	AX_RTTI_INFO(Font, RenderObject)
public:
	using Desc  = Font_CreateDesc;

	Font(const Desc& desc);

	const Desc&	desc() const { return _desc; }

	void getGlyphs(IArray<FontGlyph>& outGlyphs, StrView text) const;
	void getGlyphs(IArray<FontGlyph>& outGlyphs, StrViewU text) const;
	virtual void onGetGlyphs(MutSpan<FontGlyph> glyphs, StrViewU text) = 0;

	float lineHeight() const { return _lineHeight; }

protected:

	Desc _desc;
	Dict<CharU, FontGlyph> _cachedGlyphs;

	float		_lineHeight = 0;
	FontGlyph	_missingGlyph;
	bool		_hasKerning : 1;
};

#define AX_FontStyleFlags_ENUM_LIST(E) \
	E(None,) \
	E(Underline,) \
	E(Strikethrough,) \
//----
AX_ENUM_FLAGS_CLASS(AX_FontStyleFlags_ENUM_LIST, FontStyleFlags, u64)

class FontStyle : public RenderObject {
public:
	using Flags = FontStyleFlags;

	SPtr<Font> font;
	Color4f    color{1, 1, 1};
	Color4f    bgColor{0, 0, 0, 0};
	Margin2f   bgPadding{4};
	Color4f    selectedColor{1, 1, 1};
	Color4f    selectedBgColor{0, 0, 1};
	Flags      flags = Flags::None;

	AX_META_TYPE(FontStyle, RttiObject) {
		AX_META_FIELD(color) {};
		AX_META_FIELD(bgColor) {};
		AX_META_FIELD(selectedColor) {};
		AX_META_FIELD(selectedBgColor) {};
		AX_META_FIELD(flags) {};
	};

	FontStyle() = default;
	FontStyle(const FontStyle& r) {
		operator=(r);
	}

	void operator=(const FontStyle& r) {
		font            = r.font;
		color           = r.color;
		bgColor         = r.bgColor;
		selectedColor   = r.selectedColor;
		selectedBgColor = r.selectedBgColor;
		flags           = r.flags;
	}
};

class FontProvider : public RenderObject {
	AX_RTTI_INFO(FontProvider, RenderObject);
public:
	using Font = Font;

	SPtr<Font> createFont  (const Font_CreateDesc& desc);
	virtual SPtr<Font> onCreateFont(const Font_CreateDesc& desc) = 0;

	virtual FontFace* loadFontFile	(StrView filename,				StrView userDefineName) = 0;
	virtual FontFace* loadFontMem	(ByteSpan mem, bool copyMem,	StrView userDefineName) = 0;
	virtual FontFace* loadFontMem	(IByteArray && mem,				StrView userDefineName) = 0;
	
private:
};


class axUITextMesh;
class axUITextBlock;

class FontFace : public RenderObject {
	AX_RTTI_INFO(FontFace, RenderObject)
public:
	NameId name		() const { return _name; }
	NameId family	() const { return _family; }
	NameId styleName() const { return _styleName; }

protected:
	NameId _name;
	NameId _family;
	NameId _styleName;
};

class FontManager;
class axRenderContext;
class RenderRequest;

class FontGlyphCache : public NonCopyable {
public:
	void create(Int width, Int height);
	Texture2D*	getTexture() { return _tex; }

	void onDebugRender(RenderRequest& req);

	bool onAddGlyph(Font& font, FontGlyph& glyph, const ImageInfo& info, ByteSpan pixelData);

private:
	RectBinPacker_Skyline	_packer;

	SPtr<Texture2D>	_tex;
	//	Rect2i			_dirtyRect {0,0,0,0};

	ImageRb	_image;
	Vec2i	_offset {0,0};

};


class FontManager : public NonCopyable {
	using This = FontManager;
public:
	static FontManager* s_instance();

	FontManager();
	~FontManager();

	FontFace* loadFontFile	(StrView filename, StrView userDefineName = StrView::s_empty());

	SPtr<Font> createFont	(const Font_CreateDesc& desc);
	SPtr<Font> createFont	(InNameId face, Int size, bool bold = false, bool italic = false);

	Texture2D*		getGlyphTexture()	{ return _glyphCache.getTexture(); };

	void onDebugRender(RenderRequest& req);

	FontGlyphCache* glypchCache() { return &_glyphCache; }

private:
	SPtr<FontProvider>	_provider;
	FontGlyphCache		_glyphCache;
};

} // namespace