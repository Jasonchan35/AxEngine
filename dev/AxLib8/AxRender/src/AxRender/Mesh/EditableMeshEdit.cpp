module AxRender;
import :EditableMeshEdit;

namespace ax {

void EditableMeshEdit::createSphere(double radius, Int rows, Int columns) {
	_mesh.clear();

	for (Int y = 1; y < rows; y++) {
		double sinY, cosY;
		Math::sincos(Math::PI_<f64> * static_cast<f64>(y) / static_cast<f64>(rows), sinY, cosY);
		for (Int x = 0; x < columns; x++) {
			double sinX, cosX;
			Math::sincos(2 * Math::PI_<f64> * static_cast<f64>(x) / static_cast<f64>(columns), sinX, cosX);
			_mesh.addPoint(Vec3d(sinX * sinY, cosY, cosX * sinY) * radius);
		}
	}

	//top
	{
		auto s = _mesh.points().size();
		_mesh.addPoint(0, radius, 0);
		for (Int x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			Int vi[] = {s, x, x1};
			_mesh.addFace(vi);
		}
	}

	for (Int y = 0; y < rows - 2; y++) {
		for (Int x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			auto y1 = y + 1;
			Int vi[4] = {
				y  * columns + x,
				y1 * columns + x,
				y1 * columns + x1,
				y  * columns + x1,
			};
			_mesh.addFace(vi);
		}
	}

	//bottom
	{
		auto center = _mesh.points().size();
		_mesh.addPoint(0, -radius, 0);
		for (Int x = 0; x < columns; x++) {
			auto y  = rows - 2;
			auto x1 = (x + 1) % columns;

			Int vi[] = {
				y * columns + x1, 
				y * columns + x, 
				center
			};
			_mesh.addFace(vi);
		}
	}
}

void EditableMeshEdit::createCylinder(double height, double radius, Int rows, Int columns, bool topCap, bool bottomCap) {
	_mesh.clear();

	auto heightHalf = height * 0.5;
	auto rows1 = rows + 1;

	for (Int y = 0; y < rows1; y++) {
		f64 dy = static_cast<f64>(y) / static_cast<f64>(rows);
		f64 sinY, cosY;
		Math::sincos(Math::PI_<f64> * dy, sinY, cosY);
		for (Int x = 0; x < columns; x++) {
			f64 sinX, cosX;
			f64 dx = static_cast<f64>(x) / static_cast<f64>(columns);
			Math::sincos(2 * Math::PI_<f64> * dx, sinX, cosX);
			_mesh.addPoint(Vec3d(sinX * radius, dy * height - heightHalf, cosX * radius));
		}
	}

	for (Int y = 0; y < rows; y++) {
		for (Int x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			auto y1 = y + 1;
			Int vi[4] = {
				y  * columns + x,
				y1 * columns + x,
				y1 * columns + x1,
				y  * columns + x1,
			};
			_mesh.addFace(vi);
		}
	}

	if (topCap) {
		auto s = _mesh.points().size();
		_mesh.addPoint(0, heightHalf, 0);
		for (Int x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			Int vi[] = {s, x, x1};
			_mesh.addFace(vi);
		}
	}

	if (bottomCap) {
		auto center = _mesh.points().size();
		_mesh.addPoint(0, -heightHalf, 0);

		for (Int x = 0; x < columns; x++) {
			auto y  = rows;
			auto x1 = (x + 1) % columns;

			Int vi[] = {
				y * columns + x1, 
				y * columns + x, 
				center
			};
			_mesh.addFace(vi);
		}
	}
}

void EditableMeshEdit::createCone(double height, double radius, Int rows, Int columns, bool bottomCap/*=true*/) {
	_mesh.clear();

	auto heightHalf = height * 0.5;
	auto rows1 = rows + 1;

	for (Int y = 1; y < rows1; y++) {
		f64 dy = static_cast<f64>(y) / static_cast<f64>(rows);
		f64 sinY, cosY;
		Math::sincos(Math::PI_<f64> * dy, sinY, cosY);
		for (Int x = 0; x < columns; x++) {
			f64 sinX, cosX;
			f64 dx = static_cast<f64>(x) / static_cast<f64>(columns);
			Math::sincos(2 * Math::PI_<f64> * dx, sinX, cosX);
			_mesh.addPoint(Vec3d(sinX * radius * dy, dy * -height + heightHalf, cosX * radius * dy));
		}
	}

	for (Int y = 0; y < rows-1; y++) {
		for (Int x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			auto y1 = y + 1;
			Int vi[4] = {
				y  * columns + x,
				y1 * columns + x,
				y1 * columns + x1,
				y  * columns + x1,
			};
			_mesh.addFace(vi);
		}
	}

	//top
	{
		auto s = _mesh.points().size();
		_mesh.addPoint(0, heightHalf, 0);
		for (Int x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			Int vi[] = {s, x, x1};
			_mesh.addFace(vi);
		}
	}

	if (bottomCap) {
		auto center = _mesh.points().size();
		_mesh.addPoint(0, -heightHalf, 0);

		for (Int x = 0; x < columns; x++) {
			auto y  = rows - 1;
			auto x1 = (x + 1) % columns;

			Int vi[] = {
				y * columns + x1, 
				y * columns + x, 
				center
			};
			_mesh.addFace(vi);
		}
	}
}

} // namespace
