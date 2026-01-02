module AxRender;
import :RectBinPacker;

namespace ax {
// +: skyline points
//
// +----------------------------------------+
// |                                        |
// |                                        |
// |                                        |
// |                                        |
// |                                        |
// |          +------+                      |
// |          |      |          +------+    |
// |+----+    |      |          |      |    |
// |     |    |      +----------+      |    |
// |     +----+                        +----|
// |                                        |
// +----------------------------------------+

RectBinPacker_Skyline::RectBinPacker_Skyline() {
	_skyline     = &_skylineBuf[0];
	_skylineBack = &_skylineBuf[1];
}

void RectBinPacker_Skyline::create(Int width, Int height) {
	_width  = width;
	_height = height;

	auto& skyline = *_skyline;

	skyline.resize(2);
	skyline.at(0) = Vec2i(0,	 0);
	skyline.at(1) = Vec2i(width, 0);
}

RectBinPacker::Result RectBinPacker_Skyline::onAddRect(const Vec2i& reqSize) {
	Result result;

	auto& skyline = *_skyline;
	if (!skyline.size()) {
		throw Error_Undefined();
	}

	Int n = skyline.size();
	Int minY = _height;
	Int idx = -1;
	for (Int i = 0; i < n-1; i++) {
		auto s = skyline[i];

		auto tmpX = s.x + reqSize.x;
		if (tmpX > _width) break;

		//find max y in overlapped skyline segments in x axis
		auto tmpY = s.y;
		for (Int j = i+1; j < n; j++) {
			auto q = skyline[j];
			if (q.x >= tmpX) break;
			if (q.y > tmpY) {
				tmpY = q.y;
			}
		}

		if (tmpY + reqSize.y > _height) break;

		if (tmpY < minY) {
			idx = i;
			minY = tmpY;
		}
	}	

	if (idx < 0) {
		result.rect.set(0, 0, 0, 0);
	}else{
		Rect2i rect(skyline[idx].x, minY, reqSize.x, reqSize.y);
		_updateSkyline(idx, rect);
		//AX_LOG("--");
		//AX_DUMP_VAR(rect);
		//AX_DUMP_VAR(*_skyline);

		result.rect = rect;
	}

	return result;
}

void RectBinPacker_Skyline::_updateSkyline(Int idx, const Rect2i& rect) {
	auto& skyline    = *_skyline;
	auto& newSkyline = *_skylineBack;

	Int n = skyline.size();
	newSkyline.clear();
	newSkyline.ensureCapacity(n + 1);

	Int xMax = rect.xMax();
	Int yMax = rect.yMax();

	// copy all before idx
	for (Int j = 0; j < idx; j++) {
		auto q = skyline[j];
		_addPointToNewSkyline(q);
	}

	// add new point
	{
		auto q = Vec2i(rect.x, yMax);
		_addPointToNewSkyline(q);
	}

	// bypass all overlapped points
	for (Int j = idx; j < n-1; j++) {
		auto s = skyline[j];
		if (s.x >= xMax) break;

		idx = j;
	}

	// add new point
	{
		auto q = Vec2i(xMax, rect.y);
		_addPointToNewSkyline(q);
	}

	idx++;

	for (Int j = idx; j < n-1; j++) {
		auto q = skyline[j];
		_addPointToNewSkyline(q);
	}

	//always add last one
	newSkyline.append(skyline.back());

	std::swap(_skyline, _skylineBack);
}

void RectBinPacker_Skyline::_addPointToNewSkyline(const Vec2i& q) {
	auto& newSkyline = *_skylineBack;

	if (newSkyline.size() > 0) {
		if (newSkyline.back().y == q.y) {
			return; //skip if y as same as last one
		}
	}

	newSkyline.append( q );
}

} // namespace