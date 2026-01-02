module;

export module AxRender:RectBinPacker;
import :Common;

export namespace ax {

class RectBinPacker : public NonCopyable {
public:
	virtual ~RectBinPacker() = default;
	struct Result {
		Rect2i	rect;
		bool		rotated = false;
	};
	
	Result addRect(const Vec2i& reqSize) { return onAddRect(reqSize); }
protected:	
	virtual Result onAddRect(const Vec2i& reqSize) = 0;
};

// http://clb.demon.fi/files/RectangleBinPack.pdf
// The Skyline Algorithms
	
class RectBinPacker_Skyline : public RectBinPacker {
public:
	RectBinPacker_Skyline();
	void create(Int width, Int height);

	virtual Result onAddRect(const Vec2i& reqSize) override;

	Span<Vec2i>	skyline() const { return *_skyline; }

private:
	void _updateSkyline(Int idx, const Rect2i& rect);
	void _addPointToNewSkyline(const Vec2i& pt);

	Int	_width;
	Int	_height;

	Array<Vec2i>*	_skyline     = nullptr;
	Array<Vec2i>*	_skylineBack = nullptr;
	Array<Vec2i>	_skylineBuf[2];
};


} // namespace